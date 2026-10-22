#include "PluginEditor.h"
#include "DualfatFonts.h"
namespace
{
const juce::Colour bg(0xff090b10), panel(0xff141820), ink(0xffe8edf5), muted(0xff788493),
    cyan(0xff43e5ed), violet(0xffad81ff), edge(0xff252c36);
juce::Typeface::Ptr roboto(bool bold = false)
{
    static auto regular = juce::Typeface::createSystemTypefaceFor(
        DualfatFonts::RobotoRegular_ttf, DualfatFonts::RobotoRegular_ttfSize);
    static auto heavy = juce::Typeface::createSystemTypefaceFor(DualfatFonts::RobotoBold_ttf,
                                                                DualfatFonts::RobotoBold_ttfSize);
    return bold ? heavy : regular;
}
juce::Font uiFont(float size, bool bold = false)
{
    return juce::Font(juce::FontOptions(roboto(bold)).withHeight(size));
}
void text(juce::Graphics &g, const juce::String &s, juce::Rectangle<int> r, float size,
          juce::Colour c = ink, juce::Justification j = juce::Justification::centredLeft,
          bool bold = false)
{
    g.setColour(c);
    g.setFont(uiFont(size, bold));
    g.drawText(s, r, j);
}
void led(juce::Graphics &g, juce::Point<float> centre, float radius, juce::Colour colour,
         float power)
{
    power = juce::jlimit(0.0f, 1.0f, power);
    float halo = radius * 4;
    g.setGradientFill(juce::ColourGradient(colour.withAlpha(power * .32f), centre,
                                           colour.withAlpha(0.0f), centre.translated(halo, 0),
                                           true));
    g.fillEllipse(centre.x - halo, centre.y - halo, halo * 2, halo * 2);
    g.setColour(juce::Colour(0xff030508));
    g.fillEllipse(centre.x - radius - 1, centre.y - radius - 1, radius * 2 + 2, radius * 2 + 2);
    g.setColour(colour.withMultipliedBrightness(.12f + power * .88f));
    g.fillEllipse(centre.x - radius, centre.y - radius, radius * 2, radius * 2);
    g.setColour(juce::Colours::white.withAlpha(power * .7f));
    g.fillEllipse(centre.x - radius * .45f, centre.y - radius * .6f, radius * .6f, radius * .6f);
}
void well(juce::Graphics &g, juce::Rectangle<float> r, float radius = 8.0f)
{
    g.setColour(juce::Colours::black.withAlpha(.55f));
    g.fillRoundedRectangle(r.translated(0, -1), radius);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff07090e), r.getTopLeft(),
                                           juce::Colour(0xff10151c), r.getBottomRight(), false));
    g.fillRoundedRectangle(r.reduced(1), radius);
    g.setColour(juce::Colour(0xff262c36));
    g.drawRoundedRectangle(r, radius, .8f);
    g.setColour(juce::Colours::white.withAlpha(.025f));
    g.drawHorizontalLine(int(r.getBottom()), r.getX() + radius, r.getRight() - radius);
}
} // namespace
CobaltLook::CobaltLook()
{
    setDefaultSansSerifTypeface(roboto());
    setColour(juce::Slider::textBoxTextColourId, ink);
    setColour(juce::Slider::textBoxOutlineColourId, juce::Colour(0xff242b35));
    setColour(juce::Slider::textBoxBackgroundColourId, juce::Colour(0xff0a0d12));
    setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xff07090d));
    setColour(juce::TextEditor::textColourId, ink);
    setColour(juce::TextEditor::highlightColourId, cyan.withAlpha(.25f));
    setColour(juce::ComboBox::backgroundColourId, juce::Colour(0xff0a0d12));
    setColour(juce::ComboBox::textColourId, ink);
    setColour(juce::ComboBox::outlineColourId, edge);
    setColour(juce::PopupMenu::backgroundColourId, juce::Colour(0xff10141b));
    setColour(juce::PopupMenu::textColourId, ink);
    setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xff24343e));
    setColour(juce::PopupMenu::highlightedTextColourId, cyan);
    setColour(juce::ToggleButton::textColourId, muted);
    setColour(juce::ToggleButton::tickColourId, cyan);
    setColour(juce::TextButton::buttonColourId, panel);
    setColour(juce::TextButton::textColourOffId, ink);
}
juce::Typeface::Ptr CobaltLook::getTypefaceForFont(const juce::Font &f)
{
    return roboto(f.isBold());
}
juce::Font CobaltLook::getTextButtonFont(juce::TextButton &, int)
{
    return uiFont(11, true);
}
juce::Font CobaltLook::getComboBoxFont(juce::ComboBox &)
{
    return uiFont(12);
}
juce::Font CobaltLook::getPopupMenuFont()
{
    return uiFont(13);
}
juce::Label *CobaltLook::createSliderTextBox(juce::Slider &s)
{
    auto *label = juce::LookAndFeel_V4::createSliderTextBox(s);
    label->setFont(uiFont(12, true));
    label->setColour(juce::Label::outlineColourId, edge.withAlpha(.7f));
    return label;
}
void CobaltLook::drawLabel(juce::Graphics &g, juce::Label &label)
{
    auto bounds = label.getLocalBounds();
    if (label.findParentComponentOfClass<juce::Slider>() != nullptr)
    {
        well(g, bounds.toFloat().reduced(.5f), 5);
        if (!label.isBeingEdited())
            text(g, label.getText(), bounds.reduced(3, 0), 12, ink, juce::Justification::centred,
                 true);
        return;
    }
    if (!label.isBeingEdited())
        text(g, label.getText(), label.getBorderSize().subtractedFrom(bounds), label.getFont().getHeight(),
             label.findColour(juce::Label::textColourId), label.getJustificationType(),
             label.getFont().isBold());
}
void CobaltLook::drawTooltip(juce::Graphics &g, const juce::String &value, int w, int h)
{
    well(g, juce::Rectangle<float>(0, 0, float(w), float(h)), 5);
    text(g, value, {5, 2, w - 10, h - 4}, 12, ink, juce::Justification::centred);
}
void CobaltLook::drawButtonBackground(juce::Graphics &g, juce::Button &b, const juce::Colour &,
                                      bool hover, bool down)
{
    auto r = b.getLocalBounds().toFloat().reduced(2);
    if (!down)
    {
        juce::DropShadow(juce::Colours::black.withAlpha(.5f), 5, {2, 3})
            .drawForRectangle(g, r.toNearestInt());
        g.setColour(juce::Colours::white.withAlpha(.035f));
        g.fillRoundedRectangle(r.translated(-1, -1), 6);
    }
    g.setGradientFill(juce::ColourGradient(juce::Colour(down ? 0xff0b0e14 : 0xff202630),
                                           r.getTopLeft(), juce::Colour(0xff11151c),
                                           r.getBottomRight(), false));
    g.fillRoundedRectangle(r, 6);
    g.setColour(hover ? cyan.withAlpha(.45f) : edge);
    g.drawRoundedRectangle(r, 6, .8f);
}
void CobaltLook::drawToggleButton(juce::Graphics &g, juce::ToggleButton &b, bool hover, bool)
{
    auto r = b.getLocalBounds().toFloat();
    bool on = b.getToggleState();
    auto c = b.findColour(juce::ToggleButton::tickColourId);
    auto track = juce::Rectangle<float>(1, r.getCentreY() - 7, 27, 14);
    well(g, track, 7);
    if (on)
    {
        g.setColour(c.withAlpha(.18f));
        g.fillRoundedRectangle(track.reduced(2), 5);
    }
    led(g, {on ? 21.0f : 8.0f, r.getCentreY()}, 3.3f, c, on ? 1.0f : .12f);
    text(g, b.getButtonText(), {35, 0, b.getWidth() - 35, b.getHeight()}, 10.5f,
         on ? ink : (hover ? ink.withAlpha(.75f) : muted), juce::Justification::centredLeft, true);
}
void CobaltLook::drawComboBox(juce::Graphics &g, int w, int h, bool down, int, int, int, int,
                              juce::ComboBox &box)
{
    well(g, juce::Rectangle<float>(1, 1, float(w - 2), float(h - 2)), 5);
    if (down || box.isMouseOverOrDragging())
    {
        g.setColour(box.findColour(juce::ComboBox::arrowColourId).withAlpha(.35f));
        g.drawRoundedRectangle(juce::Rectangle<float>(1, 1, float(w - 2), float(h - 2)), 5, 1);
    }
    juce::Path arrow;
    arrow.startNewSubPath(float(w - 19), float(h) * .43f);
    arrow.lineTo(float(w - 15), float(h) * .58f);
    arrow.lineTo(float(w - 11), float(h) * .43f);
    g.setColour(muted);
    g.strokePath(arrow, juce::PathStrokeType(1.3f, juce::PathStrokeType::curved,
                                             juce::PathStrokeType::rounded));
}
void CobaltLook::drawLinearSlider(juce::Graphics &g, int x, int y, int w, int h, float sliderPos,
                                  float, float, juce::Slider::SliderStyle, juce::Slider &s)
{
    auto c = s.findColour(juce::Slider::trackColourId);
    auto r = juce::Rectangle<float>(float(x), float(y + h / 2 - 3), float(w), 6);
    well(g, r.expanded(1), 4);
    float p = juce::jlimit(0.0f, 1.0f, (sliderPos - float(x)) / float(juce::jmax(1, w)));
    auto active = r.withWidth(juce::jmax(1.0f, sliderPos - float(x)));
    g.setColour(c.withAlpha(.06f));
    g.fillRoundedRectangle(active.expanded(2), 5);
    g.setGradientFill(
        juce::ColourGradient(c.withAlpha(.3f), r.getTopLeft(), c, active.getTopRight(), false));
    g.fillRoundedRectangle(active.reduced(0, 1), 2);
    auto knob = juce::Rectangle<float>(14, 14).withCentre({sliderPos, r.getCentreY()});
    g.setColour(juce::Colour(0xff040609));
    g.fillEllipse(knob.expanded(1));
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff46505d), knob.getTopLeft(),
                                           juce::Colour(0xff161c24), knob.getBottomRight(), false));
    g.fillEllipse(knob);
    g.setColour(edge.brighter(.2f));
    g.drawEllipse(knob, .8f);
    led(g, knob.getCentre(), 2.2f, c, .3f + p * .7f);
}
const juce::Image &CobaltLook::dialBase(int diameter)
{
    auto found = dialImages.find(diameter);
    if (found != dialImages.end())
        return found->second;
    constexpr int margin = 24;
    juce::Image image(juce::Image::ARGB, (diameter + margin * 2) * 2, (diameter + margin * 2) * 2,
                      true);
    juce::Graphics g(image);
    g.addTransform(juce::AffineTransform::scale(2));
    auto r = juce::Rectangle<float>(float(margin), float(margin), float(diameter), float(diameter));
    juce::Path outer;
    outer.addEllipse(r);
    juce::DropShadow(juce::Colours::black.withAlpha(.9f), 13, {5, 8}).drawForPath(g, outer);
    juce::DropShadow(juce::Colour(0xff4b5564).withAlpha(.19f), 9, {-4, -4}).drawForPath(g, outer);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff030509), r.getTopLeft(),
                                           juce::Colour(0xff2a313b), r.getBottomRight(), false));
    g.fillEllipse(r);
    g.setColour(juce::Colour(0xff303744));
    g.drawEllipse(r.reduced(.7f), .8f);
    g.setColour(juce::Colour(0xff04070c));
    g.fillEllipse(r.reduced(float(diameter) * .035f));
    auto bezel = r.reduced(float(diameter) * .115f);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff687482), bezel.getTopLeft(),
                                           juce::Colour(0xff11151d), bezel.getBottomRight(),
                                           false));
    g.fillEllipse(bezel);
    auto face = bezel.reduced(2);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff303844), face.getTopLeft(),
                                           juce::Colour(0xff0c1017), face.getBottomRight(), false));
    g.fillEllipse(face);
    auto inside = face.reduced(float(diameter) * .035f);
    g.setGradientFill(
        juce::ColourGradient(juce::Colour(0xff272e39), inside.getX() + inside.getWidth() * .28f,
                             inside.getY() + inside.getHeight() * .18f, juce::Colour(0xff10151d),
                             inside.getRight(), inside.getBottom(), true));
    g.fillEllipse(inside);
    g.setColour(juce::Colours::white.withAlpha(.035f));
    g.drawEllipse(inside.reduced(.5f), .65f);
    if (dialImages.size() >= 12)
        dialImages.erase(dialImages.begin());
    return dialImages.emplace(diameter, std::move(image)).first->second;
}
void CobaltLook::drawRotarySlider(juce::Graphics &g, int x, int y, int w, int h, float p,
                                  float start, float end, juce::Slider &s)
{
    auto bounds = juce::Rectangle<float>(float(x), float(y), float(w), float(h))
                      .reduced(w > 150 ? 12.0f : 5.0f);
    float size = std::min(bounds.getWidth(), bounds.getHeight());
    if (size < 5)
        return;
    auto r = juce::Rectangle<float>(size, size).withCentre(bounds.getCentre());
    auto centre = r.getCentre();
    auto c = s.findColour(juce::Slider::rotarySliderFillColourId);
    float shown = float(s.getProperties().getWithDefault("visualValue", p));
    float power = float(s.getProperties().getWithDefault("ledPower", 1.0f));
    g.drawImage(dialBase(juce::roundToInt(size)), r.expanded(24));
    float radius = size * .444f;
    float brightness = (.35f + shown * .65f) * power;
    juce::Path arc;
    arc.addCentredArc(centre.x, centre.y, radius, radius, 0, start, start + shown * (end - start),
                      true);
    for (auto glow : {std::pair<float, float>{16, .025f}, {10, .07f}, {5, .17f}})
    {
        g.setColour(c.withAlpha(brightness * glow.second));
        g.strokePath(arc, juce::PathStrokeType(glow.first, juce::PathStrokeType::curved,
                                               juce::PathStrokeType::rounded));
    }
    g.setColour(c.withAlpha(.75f * power));
    g.strokePath(arc, juce::PathStrokeType(size > 110 ? 2.7f : 2.0f, juce::PathStrokeType::curved,
                                           juce::PathStrokeType::rounded));
    int ticks = size > 110 ? 49 : 25;
    for (int i = 0; i < ticks; ++i)
    {
        float t = float(i) / float(ticks - 1), angle = start + t * (end - start);
        auto position = centre.getPointOnCircumference(size * .481f, angle);
        bool on = t <= shown;
        g.setColour(on ? c.withAlpha((.3f + .7f * t) * power) : juce::Colour(0xff333a44));
        float dot = size > 110 ? 1.15f : .8f;
        g.fillEllipse(position.x - dot, position.y - dot, dot * 2, dot * 2);
    }
    float angle = start + shown * (end - start);
    auto tip = centre.getPointOnCircumference(size * .303f, angle);
    auto root = centre.getPointOnCircumference(size * .11f, angle);
    g.setColour(juce::Colours::black.withAlpha(.6f));
    g.drawLine({root.translated(1, 2), tip.translated(1, 2)}, size > 110 ? 5 : 3);
    g.setColour(c.withAlpha(.35f + .65f * power));
    g.drawLine({root, tip}, size > 110 ? 3 : 2);
    led(g, tip, size > 110 ? 2.5f : 1.7f, c, brightness);
    if (s.getProperties().contains("modulationPosition"))
    {
        float position = float(s.getProperties()["modulationPosition"]);
        led(g, centre.getPointOnCircumference(radius, start + position * (end - start)), 2.2f,
            juce::Colours::white, power * .85f);
    }
    if (size > 140)
    {
        double value = s.getValue();
        auto readout =
            value >= 1000 ? juce::String(value / 1000, 2) : juce::String(juce::roundToInt(value));
        text(g, readout, r.toNearestInt().withY(int(centre.y + 14)).withHeight(29), 25, ink,
             juce::Justification::centred, true);
        text(g, value >= 1000 ? "kHz" : "Hz",
             r.toNearestInt().withY(int(centre.y + 42)).withHeight(17), 10, muted,
             juce::Justification::centred, true);
    }
}

FilterPanel::FilterPanel(CobaltProcessor &p, int i)
    : processor(p), index(i), accent(i ? violet : cyan), prefix(i ? "lp_" : "hp_")
{
    addSlider("cutoff", "CUTOFF / Hz");
    addSlider("q", "RESONANCE / Q");
    addSlider("rate", "FREE RATE / Hz");
    addSlider("depth", "DEPTH / 5 OCT");
    addSlider("phase", "PHASE / CYCLE");
    addCombo("slope", {"12 dB/oct", "24 dB/oct", "36 dB/oct", "48 dB/oct"});
    addCombo("wave", {"Sine", "Triangle", "Saw up", "Saw down", "Square", "S&H"});
    addCombo("division", {"1/1", "1/2", "1/4", "1/8", "1/16", "1/32"});
    addCombo("timing", {"Straight", "Dotted", "Triplet"});
    addCombo("polarity", {"Bipolar", "Positive", "Negative"});
    addToggle("enabled", "ENABLE");
    addToggle("bypass", "BYPASS");
    addToggle("lfo", "LFO");
    addToggle("sync", "SYNC");
    addToggle("retrigger", "RETRIG");
}
void FilterPanel::addSlider(const char *id, const char *title)
{
    auto s = std::make_unique<juce::Slider>();
    s->setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    s->setTextBoxStyle(juce::Slider::TextBoxBelow, false, 88, 21);
    s->setColour(juce::Slider::rotarySliderFillColourId, accent);
    s->setName(title);
    s->setTooltip(juce::String(title) + " — drag or type a value; double-click to reset");
    s->setDoubleClickReturnValue(
        true, processor.state.getParameter(prefix + id)
                  ->convertFrom0to1(processor.state.getParameter(prefix + id)->getDefaultValue()));
    auto l = std::make_unique<juce::Label>();
    l->setText(title, juce::dontSendNotification);
    l->setJustificationType(juce::Justification::centred);
    l->setColour(juce::Label::textColourId, muted);
    l->setFont(uiFont(10, true));
    addAndMakeVisible(*s);
    addAndMakeVisible(*l);
    sa.push_back(std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(
        processor.state, prefix + id, *s));
    // Attachments install their own formatters; apply display formatting afterwards.
    s->textFromValueFunction = [name = juce::String(id)](double v)
    { return name == "cutoff" ? juce::String(juce::roundToInt(v)) : juce::String(v, 2); };
    s->updateText();
    s->getProperties().set("visualValue",
                           float(s->getNormalisableRange().convertTo0to1(s->getValue())));
    sliders.push_back(std::move(s));
    labels.push_back(std::move(l));
}
void FilterPanel::addCombo(const char *id, juce::StringArray values)
{
    auto c = std::make_unique<juce::ComboBox>();
    c->addItemList(values, 1);
    c->setName(id);
    c->setColour(juce::ComboBox::arrowColourId, accent);
    c->setTooltip(id);
    addAndMakeVisible(*c);
    ca.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ComboBoxAttachment>(
        processor.state, prefix + id, *c));
    combos.push_back(std::move(c));
}
void FilterPanel::addToggle(const char *id, const char *name)
{
    auto b = std::make_unique<juce::ToggleButton>(name);
    b->setColour(juce::ToggleButton::tickColourId, accent);
    addAndMakeVisible(*b);
    ba.push_back(std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(
        processor.state, prefix + id, *b));
    toggles.push_back(std::move(b));
}
void FilterPanel::resized()
{
    float scale = float(getHeight()) / 568.0f;
    auto rect = [&](int x, int y, int w, int h)
    {
        return juce::Rectangle<int>(x, juce::roundToInt(y * scale), w, juce::roundToInt(h * scale));
    };
    int w = getWidth(), mid = w / 2;
    int cut = std::min(w - 40, 250);
    sliders[0]->setBounds(rect(mid - cut / 2, 37, cut, 226));
    labels[0]->setBounds(rect(mid - 90, 265, 180, 20));
    sliders[1]->setBounds(rect(18, 278, 120, 116));
    labels[1]->setBounds(rect(12, 393, 140, 20));
    combos[0]->setBounds(rect(w - 168, 323, 146, 30));
    toggles[0]->setBounds(rect(w - 210, 9, 110, 26));
    toggles[1]->setBounds(rect(w - 104, 9, 100, 26));
    for (int i = 2; i < 5; ++i)
    {
        int cw = (w - 32) / 3;
        sliders[i]->setBounds(rect(16 + (i - 2) * cw, 470, cw, 76));
        labels[i]->setBounds(rect(16 + (i - 2) * cw, 546, cw, 18));
    }
    toggles[2]->setBounds(rect(16, 416, 90, 25));
    toggles[3]->setBounds(rect(106, 416, 112, 25));
    toggles[4]->setBounds(rect(w - 116, 416, 108, 25));
    int cw = (w - 38) / 4;
    for (int i = 1; i < 5; ++i)
        combos[i]->setBounds(rect(16 + (i - 1) * (cw + 2), 445, cw, 25));
}
void FilterPanel::animate()
{
    bool active = processor.value(prefix + "enabled") > .5f &&
                  processor.value(prefix + "bypass") < .5f && processor.value("bypass") < .5f;
    for (auto &slider : sliders)
    {
        float target = float(slider->getNormalisableRange().convertTo0to1(slider->getValue()));
        float prior = float(slider->getProperties().getWithDefault("visualValue", target));
        slider->getProperties().set("visualValue", prior + .38f * (target - prior));
        slider->getProperties().set("ledPower", active ? 1.0f : .18f);
    }
    if (processor.value(prefix + "lfo") > .5f && active)
    {
        float cutoff = processor.liveCutoff[index].load();
        sliders[0]->getProperties().set(
            "modulationPosition",
            float(cobalt::clamp(std::log(cutoff / 20.0) / std::log(1000.0), 0, 1)));
    }
    else
        sliders[0]->getProperties().remove("modulationPosition");
}
void FilterPanel::paint(juce::Graphics &g)
{
    auto r = getLocalBounds().toFloat().reduced(2);
    g.setColour(juce::Colours::black);
    g.fillRoundedRectangle(r, 17);
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff1a2029), r.getTopLeft(),
                                           juce::Colour(0xff10141b), r.getBottomRight(), false));
    g.fillRoundedRectangle(r.reduced(1), 16);
    g.setColour(edge);
    g.drawRoundedRectangle(r, 17, 1);
    g.setColour(juce::Colours::white.withAlpha(.045f));
    g.drawLine(r.getX() + 18, r.getY() + 1, r.getRight() - 18, r.getY() + 1, 1);
    text(g, index ? "02  /  LOW PASS" : "01  /  HIGH PASS", {22, 12, 175, 22}, 11, accent,
         juce::Justification::centredLeft, true);
    g.setColour(accent.withAlpha(.5f));
    g.fillRoundedRectangle(22, 37, 26, 2, 1);
    float sy = float(getHeight()) / 568;
    auto wave = juce::Rectangle<float>(155, 366 * sy, float(getWidth() - 175), 42 * sy);
    well(g, wave, 7);
    text(g, "MODULATOR", {157, int(347 * sy), 100, 16}, 9, muted, juce::Justification::centredLeft,
         true);
    bool on = processor.value(prefix + "lfo") > .5f;
    led(g, {wave.getRight() - 8, wave.getY() - 11 * sy}, 2, accent, on ? .9f : .08f);
    juce::Path path;
    int shape = int(processor.value(prefix + "wave"));
    double phase = processor.livePhase[index].load() + processor.value(prefix + "phase");
    auto scope = wave.reduced(8, 6);
    g.setColour(accent.withAlpha(.08f));
    g.drawHorizontalLine(int(scope.getCentreY()), scope.getX(), scope.getRight());
    for (int i = 0; i <= 180; ++i)
    {
        float x = scope.getX() + float(i) / 180 * scope.getWidth();
        float y = scope.getCentreY() -
                  float(cobalt::LFO::wave(shape, phase + double(i) / 90,
                                          cobalt::LFO::sampleHold(phase + double(i) / 90))) *
                      scope.getHeight() * .40f;
        if (i == 0)
            path.startNewSubPath(x, y);
        else
            path.lineTo(x, y);
    }
    g.setColour(accent.withAlpha(on ? .09f : .02f));
    g.strokePath(path, juce::PathStrokeType(7));
    g.setColour(accent.withAlpha(on ? .95f : .25f));
    g.strokePath(path, juce::PathStrokeType(1.6f));
    text(g, "SLOPE", {getWidth() - 168, int(300 * sy), 140, 18}, 9, muted,
         juce::Justification::centredLeft, true);
    g.setColour(juce::Colours::black.withAlpha(.5f));
    g.drawHorizontalLine(int(415 * sy), 18, float(getWidth() - 18));
    g.setColour(juce::Colours::white.withAlpha(.035f));
    g.drawHorizontalLine(int(415 * sy) + 1, 18, float(getWidth() - 18));
}
juce::Rectangle<float> ResponseGraph::plot() const
{
    return getLocalBounds().toFloat().reduced(34, 33);
}
float ResponseGraph::xFor(double f) const
{
    return plot().getX() + float(std::log(f / 20) / std::log(1000)) * plot().getWidth();
}
float ResponseGraph::yFor(double db) const
{
    return plot().getY() + float((24 - cobalt::clamp(db, -60, 24)) / 84) * plot().getHeight();
}
void ResponseGraph::paint(juce::Graphics &g)
{
    well(g, getLocalBounds().toFloat().reduced(1), 12);
    auto r = plot();
    text(g, "FILTER RESPONSE", {22, 9, 170, 18}, 10, ink, juce::Justification::centredLeft, true);
    text(g, "DRAG TO TUNE", {184, 9, 180, 18}, 9, muted);
    int legendX = getWidth() - 210;
    led(g, {float(legendX), 18}, 2, cyan, .8f);
    text(g, "HPF", {legendX + 10, 10, 40, 16}, 9, muted);
    led(g, {float(legendX + 62), 18}, 2, violet, .8f);
    text(g, "LPF", {legendX + 72, 10, 40, 16}, 9, muted);
    led(g, {float(legendX + 124), 18}, 2, ink, .65f);
    text(g, "SUM", {legendX + 134, 10, 50, 16}, 9, muted);
    for (double f : {20., 50., 100., 200., 500., 1000., 2000., 5000., 10000., 20000.})
    {
        g.setColour(juce::Colour(0xff202730));
        g.drawVerticalLine(int(xFor(f)), r.getY(), r.getBottom());
    }
    for (int db : {-60, -36, -12, 0, 24})
    {
        g.setColour(juce::Colour(0xff202730));
        g.drawHorizontalLine(int(yFor(db)), r.getX(), r.getRight());
        text(g, juce::String(db), {0, int(yFor(db)) - 7, 31, 14}, 8, muted.withAlpha(.65f),
             juce::Justification::centred);
    }
    text(g, "20 Hz", {int(r.getX()), getHeight() - 22, 70, 16}, 9, muted);
    text(g, "1 kHz", {int(xFor(1000)) - 25, getHeight() - 22, 50, 16}, 9, muted,
         juce::Justification::centred);
    text(g, "20 kHz", {getWidth() - 99, getHeight() - 22, 65, 16}, 9, muted,
         juce::Justification::centredRight);
    double sr = processor.liveRate.load();
    double cutoff[] = {processor.liveCutoff[0].load(), processor.liveCutoff[1].load()};
    juce::Path paths[3];
    for (int x = 0; x <= int(r.getWidth()); ++x)
    {
        double f = 20 * std::pow(1000, double(x) / r.getWidth());
        std::complex<double> values[2];
        for (int i = 0; i < 2; ++i)
        {
            juce::String prefix = i ? "lp_" : "hp_";
            bool enabled = processor.value(prefix + "enabled") > .5f &&
                           processor.value(prefix + "bypass") < .5f;
            values[i] =
                enabled
                    ? cobalt::Filter::response(i == 0, f, cutoff[i], processor.value(prefix + "q"),
                                               sr, int(processor.value(prefix + "slope")) + 1)
                    : std::complex<double>(1);
        }
        for (int i = 0; i < 3; ++i)
        {
            double magnitude = std::abs(i < 2 ? values[i] : values[0] * values[1]);
            float y = yFor(20 * std::log10(std::max(magnitude, 1e-8)));
            if (x == 0)
                paths[i].startNewSubPath(r.getX(), y);
            else
                paths[i].lineTo(r.getX() + float(x), y);
        }
    }
    auto fill = paths[2];
    fill.lineTo(r.getBottomRight());
    fill.lineTo(r.getBottomLeft());
    fill.closeSubPath();
    g.setGradientFill(juce::ColourGradient(cyan.withAlpha(.065f), r.getTopLeft(),
                                           violet.withAlpha(.025f), r.getBottomRight(), false));
    g.fillPath(fill);
    for (int i = 0; i < 3; ++i)
    {
        auto colour = i == 0 ? cyan : i == 1 ? violet : ink;
        g.setColour(colour.withAlpha(i == 2 ? .04f : .07f));
        g.strokePath(paths[i], juce::PathStrokeType(i == 2 ? 7 : 5));
        g.setColour(colour.withAlpha(i == 2 ? .95f : .7f));
        g.strokePath(paths[i], juce::PathStrokeType(i == 2 ? 1.8f : 1.1f));
    }
    for (int i = 0; i < 2; ++i)
    {
        juce::String prefix = i ? "lp_" : "hp_";
        auto response =
            cobalt::Filter::response(i == 0, cutoff[i], cutoff[i], processor.value(prefix + "q"),
                                     sr, int(processor.value(prefix + "slope")) + 1);
        led(g, {xFor(cutoff[i]), yFor(20 * std::log10(std::max(std::abs(response), 1e-8)))}, 3.5f,
            i ? violet : cyan, 1);
    }
}
void ResponseGraph::mouseDown(const juce::MouseEvent &e)
{
    selected = std::abs(e.position.x - xFor(processor.value("hp_cutoff"))) <
                       std::abs(e.position.x - xFor(processor.value("lp_cutoff")))
                   ? 0
                   : 1;
    processor.state.getParameter(selected ? "lp_cutoff" : "hp_cutoff")->beginChangeGesture();
    mouseDrag(e);
}
void ResponseGraph::mouseDrag(const juce::MouseEvent &e)
{
    if (selected < 0)
        return;
    auto *p = processor.state.getParameter(selected ? "lp_cutoff" : "hp_cutoff");
    double norm = cobalt::clamp((e.position.x - plot().getX()) / plot().getWidth(), 0, 1);
    p->setValueNotifyingHost(p->convertTo0to1(float(20 * std::pow(1000, norm))));
}
void ResponseGraph::mouseUp(const juce::MouseEvent &)
{
    if (selected >= 0)
        processor.state.getParameter(selected ? "lp_cutoff" : "hp_cutoff")->endChangeGesture();
    selected = -1;
}
CobaltContent::CobaltContent(CobaltProcessor &p)
    : processor(p), graph(p), hp(p, 0), lp(p, 1)
{
    brandLogo = juce::ImageCache::getFromMemory(DualfatFonts::DUALFATlogo_png,
                                               DualfatFonts::DUALFATlogo_pngSize)
                    .getClippedImage({40, 330, 1174, 510});
    setLookAndFeel(&look);
    for (auto *c : std::initializer_list<juce::Component *>{&graph, &hp, &lp, &input, &output, &mix,
                                                            &inputLabel, &outputLabel, &mixLabel,
                                                            &bypass, &save, &load, &uiSize})
        addAndMakeVisible(c);
    for (auto *s : {&input, &output, &mix})
    {
        s->setSliderStyle(juce::Slider::LinearHorizontal);
        s->setTextBoxStyle(juce::Slider::TextBoxRight, false, 65, 22);
        s->setColour(juce::Slider::trackColourId, cyan);
    }
    input.setColour(juce::Slider::trackColourId, cyan);
    output.setColour(juce::Slider::trackColourId, violet);
    mix.setColour(juce::Slider::trackColourId, ink);
    bypass.setColour(juce::ToggleButton::tickColourId, juce::Colour(0xffffbc68));
    input.setTextValueSuffix(" dB");
    output.setTextValueSuffix(" dB");
    inputLabel.setText("INPUT", juce::dontSendNotification);
    outputLabel.setText("OUTPUT", juce::dontSendNotification);
    mixLabel.setText("DRY / WET", juce::dontSendNotification);
    for (auto *l : {&inputLabel, &outputLabel, &mixLabel})
    {
        l->setColour(juce::Label::textColourId, muted);
        l->setFont(uiFont(10, true));
    }
    ia = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state, "input",
                                                                                input);
    oa = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state, "output",
                                                                                output);
    ma =
        std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(p.state, "mix", mix);
    ba = std::make_unique<juce::AudioProcessorValueTreeState::ButtonAttachment>(p.state, "bypass",
                                                                                bypass);
    mix.textFromValueFunction = [](double v)
    { return juce::String(juce::roundToInt(v * 100)) + " %"; };
    mix.valueFromTextFunction = [](const juce::String &s) { return s.getDoubleValue() / 100; };
    for (auto *slider : {&input, &output})
    {
        slider->textFromValueFunction = [](double v) { return juce::String(v, 1); };
        slider->updateText();
    }
    mix.updateText();
    save.onClick = [this] { preset(true); };
    load.onClick = [this] { preset(false); };
    uiSize.setName("UI size");
    uiSize.setTooltip("Resize the entire interface, or drag the bottom-right corner");
    for (int percent : {50, 60, 75, 85, 100, 125, 150})
        uiSize.addItem("UI SIZE  " + juce::String(percent) + " %", percent);
    uiSize.onChange = [this]
    {
        if (onScaleChange && uiSize.getSelectedId() > 0)
            onScaleChange(float(uiSize.getSelectedId()) / 100.0f);
    };
    setSize(1120, 988);
    startTimerHz(30);
}
CobaltContent::~CobaltContent()
{
    stopTimer();
    setLookAndFeel(nullptr);
}
void CobaltContent::paint(juce::Graphics &g)
{
    auto r = getLocalBounds().toFloat();
    g.setGradientFill(juce::ColourGradient(juce::Colour(0xff151920), r.getTopLeft(), bg,
                                           r.getBottomRight(), false));
    g.fillRect(r);
    g.setColour(juce::Colour(0xff2b313a));
    g.drawRect(r.reduced(.5f), 1);
    g.setImageResamplingQuality(juce::Graphics::highResamplingQuality);
    g.drawImageWithin(brandLogo, 24, 10, 260, 113,
                      juce::RectanglePlacement::centred);
    bool bypassed = processor.value("bypass") > .5f;
    led(g, {float(getWidth() - 157), 72}, 2.5f, bypassed ? juce::Colour(0xffffbc68) : cyan, .9f);
    text(g, bypassed ? "BYPASSED" : "ENGINE ONLINE", {getWidth() - 145, 63, 125, 18}, 9, muted,
         juce::Justification::centredLeft, true);
    text(g, "DUAL RESONANT FILTER", {getWidth() / 2 - 120, 31, 240, 20}, 10,
         muted.withAlpha(.45f), juce::Justification::centred);
    auto footer = juce::Rectangle<float>(22, float(getHeight() - 73), float(getWidth() - 44), 56);
    well(g, footer, 9);
}
void CobaltContent::resized()
{
    int w = getWidth(), h = getHeight();
    uiSize.setBounds(w / 2 - 80, 64, 160, 28);
    save.setBounds(w - 357, 25, 88, 30);
    load.setBounds(w - 259, 25, 88, 30);
    bypass.setBounds(w - 147, 28, 124, 25);
    graph.setBounds(24, 138, w - 48, 182);
    int panelHeight = h - 420;
    hp.setBounds(22, 336, (w - 58) / 2, panelHeight);
    lp.setBounds(36 + (w - 58) / 2, 336, (w - 58) / 2, panelHeight);
    int third = (w - 76) / 3;
    inputLabel.setBounds(36, h - 66, third, 16);
    outputLabel.setBounds(36 + third + 8, h - 66, third, 16);
    mixLabel.setBounds(36 + (third + 8) * 2, h - 66, third - 8, 16);
    input.setBounds(34, h - 49, third - 10, 24);
    output.setBounds(34 + third + 8, h - 49, third - 10, 24);
    mix.setBounds(34 + (third + 8) * 2, h - 49, third - 10, 24);
}
void CobaltContent::timerCallback()
{
    hp.animate();
    lp.animate();
    graph.repaint();
    hp.repaint();
    lp.repaint();
    repaint(0, 0, getWidth(), 100);
}
void CobaltContent::preset(bool saving)
{
    chooser = std::make_unique<juce::FileChooser>(
        saving ? "Save DUALFAT preset" : "Load DUALFAT preset",
        juce::File::getSpecialLocation(juce::File::userDocumentsDirectory),
        saving ? "*.dualfat" : "*.dualfat;*.cobalt");
    juce::Component::SafePointer<CobaltContent> safe(this);
    chooser->launchAsync(
        saving
            ? (juce::FileBrowserComponent::saveMode | juce::FileBrowserComponent::canSelectFiles |
               juce::FileBrowserComponent::warnAboutOverwriting)
            : (juce::FileBrowserComponent::openMode | juce::FileBrowserComponent::canSelectFiles),
        [safe, saving](const juce::FileChooser &fc)
        {
            if (!safe)
                return;
            auto f = fc.getResult();
            if (f == juce::File())
                return;
            bool ok = saving ? safe->processor.savePreset(f.withFileExtension("dualfat"))
                             : safe->processor.loadPreset(f);
            if (!ok)
                juce::AlertWindow::showMessageBoxAsync(
                    juce::MessageBoxIconType::WarningIcon, "Preset error",
                    "Could not read or write this DUALFAT preset.");
        });
}

void CobaltContent::updateScale(float scale)
{
    uiSize.setText("UI SIZE  " + juce::String(juce::roundToInt(scale * 100.0f)) + " %",
                   juce::dontSendNotification);
}

CobaltEditor::CobaltEditor(CobaltProcessor &p)
    : AudioProcessorEditor(p), processor(p), content(p)
{
    const int width = juce::jlimit(560, 1680, processor.editorWidth.load());
    addAndMakeVisible(content);
    setResizable(true, true);
    setResizeLimits(560, 494, 1680, 1482);
    getConstrainer()->setFixedAspectRatio(1120.0 / 988.0);
    content.onScaleChange = [this](float scale)
    {
        setSize(juce::roundToInt(1120.0f * scale), juce::roundToInt(988.0f * scale));
    };
    setSize(width, juce::roundToInt(float(width) * 988.0f / 1120.0f));
}

void CobaltEditor::resized()
{
    const float scale = std::min(float(getWidth()) / 1120.0f, float(getHeight()) / 988.0f);
    const float x = (float(getWidth()) - 1120.0f * scale) * 0.5f;
    const float y = (float(getHeight()) - 988.0f * scale) * 0.5f;
    content.setTransform(juce::AffineTransform::scale(scale).translated(x, y));
    content.updateScale(scale);
    processor.editorWidth.store(juce::jlimit(560, 1680, getWidth()));
}

void CobaltEditor::paint(juce::Graphics &g)
{
    g.fillAll(bg);
}
