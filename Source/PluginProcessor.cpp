#include "PluginProcessor.h"
#include "PluginEditor.h"
namespace
{
const char *suffixes[] = {"cutoff",  "q",        "depth",  "rate", "phase",
                          "slope",   "division", "timing", "wave", "polarity",
                          "enabled", "bypass",   "lfo",    "sync", "retrigger"};
}
juce::AudioProcessorValueTreeState::ParameterLayout CobaltProcessor::layout()
{
    juce::AudioProcessorValueTreeState::ParameterLayout l;
    auto number =
        [&](juce::String id, juce::String name, float lo, float hi, float initial, bool log = false)
    {
        juce::NormalisableRange<float> r(lo, hi);
        if (log)
            r = juce::NormalisableRange<float>(
                lo, hi, [](float a, float b, float x) { return a * std::pow(b / a, x); },
                [](float a, float b, float x) { return std::log(x / a) / std::log(b / a); });
        l.add(std::make_unique<juce::AudioParameterFloat>(juce::ParameterID{id, 1}, name, r,
                                                          initial));
    };
    auto flag = [&](juce::String id, juce::String name, bool initial)
    { l.add(std::make_unique<juce::AudioParameterBool>(juce::ParameterID{id, 1}, name, initial)); };
    auto choice = [&](juce::String id, juce::String name, juce::StringArray options, int initial)
    {
        l.add(std::make_unique<juce::AudioParameterChoice>(juce::ParameterID{id, 1}, name, options,
                                                           initial));
    };
    for (int i = 0; i < 2; ++i)
    {
        juce::String p = i ? "lp_" : "hp_", n = i ? "LPF " : "HPF ";
        number(p + "cutoff", n + "Cutoff", 20, 20000, i ? 8400 : 120, true);
        number(p + "q", n + "Resonance", .5, 8, .707, true);
        number(p + "depth", n + "Depth", 0, 1, 0);
        number(p + "rate", n + "Rate Hz", .01, 20, 1, true);
        number(p + "phase", n + "Phase", 0, 1, 0);
        choice(p + "slope", n + "Slope", {"12 dB/oct", "24 dB/oct", "36 dB/oct", "48 dB/oct"}, 1);
        choice(p + "division", n + "Division", {"1/1", "1/2", "1/4", "1/8", "1/16", "1/32"}, 2);
        choice(p + "timing", n + "Timing", {"Straight", "Dotted", "Triplet"}, 0);
        choice(p + "wave", n + "Wave", {"Sine", "Triangle", "Saw up", "Saw down", "Square", "S&H"},
               0);
        choice(p + "polarity", n + "Polarity", {"Bipolar", "Positive", "Negative"}, 0);
        flag(p + "enabled", n + "Enabled", true);
        flag(p + "bypass", n + "Bypass", false);
        flag(p + "lfo", n + "LFO", false);
        flag(p + "sync", n + "Sync", true);
        flag(p + "retrigger", n + "Retrigger", true);
    }
    number("input", "Input dB", -24, 24, 0);
    number("output", "Output dB", -24, 24, 0);
    number("mix", "Dry / Wet", 0, 1, 1);
    flag("bypass", "Global Bypass", false);
    return l;
}
CobaltProcessor::CobaltProcessor()
    : AudioProcessor(BusesProperties()
                         .withInput("Input", juce::AudioChannelSet::stereo(), true)
                         .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
      state(*this, nullptr, "COBALT", layout())
{
    for (int f = 0; f < 2; ++f)
        for (int i = 0; i < 15; ++i)
            params[f].p[i] =
                state.getRawParameterValue(juce::String(f ? "lp_" : "hp_") + suffixes[i]);
    int i = 0;
    for (auto id : {"input", "output", "mix", "bypass"})
        masters[i++] = state.getRawParameterValue(id);
}
cobalt::Settings CobaltProcessor::settings(int f) const
{
    auto &p = params[f].p;
    auto v = [&](int i) { return p[i]->load(std::memory_order_relaxed); };
    cobalt::Settings s;
    s.cutoff = v(0);
    s.q = v(1);
    s.depth = v(2);
    s.rate = v(3);
    s.phase = v(4);
    s.slope = int(v(5));
    s.division = int(v(6));
    s.timing = int(v(7));
    s.wave = int(v(8));
    s.polarity = int(v(9));
    s.enabled = v(10) > .5f;
    s.bypass = v(11) > .5f;
    s.lfo = v(12) > .5f;
    s.sync = v(13) > .5f;
    s.retrigger = v(14) > .5f;
    return s;
}
void CobaltProcessor::prepareToPlay(double sr, int)
{
    cobalt::Settings s[] = {settings(0), settings(1)};
    engine.prepare(sr, s);
    liveRate = sr;
    wasPlaying = false;
    lastPpq = 0;
}
bool CobaltProcessor::isBusesLayoutSupported(const BusesLayout &b) const
{
    return b.getMainInputChannelSet() == b.getMainOutputChannelSet() &&
           (b.getMainOutputChannelSet() == juce::AudioChannelSet::stereo() ||
            b.getMainOutputChannelSet() == juce::AudioChannelSet::mono());
}
void CobaltProcessor::processBlock(juce::AudioBuffer<float> &b, juce::MidiBuffer &)
{
    process(b, false);
}
void CobaltProcessor::processBlockBypassed(juce::AudioBuffer<float> &b, juce::MidiBuffer &)
{
    process(b, true);
}
void CobaltProcessor::process(juce::AudioBuffer<float> &b, bool hostBypass)
{
    juce::ScopedNoDenormals guard;
    cobalt::Settings s[] = {settings(0), settings(1)};
    double bpm = 120, ppq = 0;
    bool playing = false, lock = false;
    if (auto *head = getPlayHead())
        if (auto pos = head->getPosition())
        {
            playing = pos->getIsPlaying();
            if (auto t = pos->getBpm())
                bpm = cobalt::clamp(*t, 1, 999);
            if (auto t = pos->getPpqPosition())
            {
                ppq = *t;
                lock = playing;
            }
        }
    bool reset = playing && (!wasPlaying || std::abs(ppq - lastPpq) > .1);
    double step = bpm / (60 * engine.sr);
    double in = std::pow(10, masters[0]->load() / 20), out = std::pow(10, masters[1]->load() / 20),
           wet = masters[2]->load();
    bool off = hostBypass || masters[3]->load() > .5f;
    auto *left = b.getWritePointer(0);
    auto *right = b.getNumChannels() > 1 ? b.getWritePointer(1) : nullptr;
    for (int i = 0; i < b.getNumSamples(); ++i)
    {
        double frame[] = {left[i], right ? right[i] : left[i]};
        engine.tick(frame, s, bpm, ppq + i * step, lock, reset && i == 0, in, out, wet, off);
        left[i] = float(frame[0]);
        if (right)
            right[i] = float(frame[1]);
    }
    wasPlaying = playing;
    lastPpq = ppq + b.getNumSamples() * step;
    for (int f = 0; f < 2; ++f)
    {
        liveCutoff[f] = float(engine.filters[f].effective);
        livePhase[f] = engine.filters[f].lfo.phase;
    }
}
void CobaltProcessor::getStateInformation(juce::MemoryBlock &b)
{
    auto xml = state.copyState().createXml();
    copyXmlToBinary(*xml, b);
}
void CobaltProcessor::setStateInformation(const void *d, int n)
{
    if (auto xml = getXmlFromBinary(d, n))
        if (xml->hasTagName(state.state.getType()))
            state.replaceState(juce::ValueTree::fromXml(*xml));
}
bool CobaltProcessor::savePreset(const juce::File &f)
{
    auto xml = state.copyState().createXml();
    return xml->writeTo(f);
}
bool CobaltProcessor::loadPreset(const juce::File &f)
{
    auto xml = juce::XmlDocument::parse(f);
    if (!xml || !xml->hasTagName(state.state.getType()))
        return false;
    auto tree = juce::ValueTree::fromXml(*xml);
    for (auto child : tree)
    {
        auto id = child["id"].toString();
        if (auto *p = state.getParameter(id))
        {
            auto v = float(child["value"]);
            if (std::isfinite(v))
            {
                p->beginChangeGesture();
                p->setValueNotifyingHost(p->convertTo0to1(v));
                p->endChangeGesture();
            }
        }
    }
    return true;
}
juce::AudioProcessorEditor *CobaltProcessor::createEditor()
{
    return new CobaltEditor(*this);
}
juce::AudioProcessor *JUCE_CALLTYPE createPluginFilter()
{
    return new CobaltProcessor;
}
