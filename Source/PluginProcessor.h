#pragma once
#include "Dsp.h"
#include <JuceHeader.h>
class CobaltProcessor : public juce::AudioProcessor
{
  public:
    CobaltProcessor();
    juce::AudioProcessorValueTreeState state;
    static juce::AudioProcessorValueTreeState::ParameterLayout layout();
    float value(const juce::String &id) const
    {
        return state.getRawParameterValue(id)->load();
    }
    void prepareToPlay(double, int) override;
    void releaseResources() override {}
    bool isBusesLayoutSupported(const BusesLayout &) const override;
    void processBlock(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    void processBlockBypassed(juce::AudioBuffer<float> &, juce::MidiBuffer &) override;
    juce::AudioProcessorEditor *createEditor() override;
    bool hasEditor() const override
    {
        return true;
    }
    const juce::String getName() const override
    {
        return "DUALFAT";
    }
    bool acceptsMidi() const override
    {
        return false;
    }
    bool producesMidi() const override
    {
        return false;
    }
    bool isMidiEffect() const override
    {
        return false;
    }
    double getTailLengthSeconds() const override
    {
        return 0;
    }
    int getNumPrograms() override
    {
        return 1;
    }
    int getCurrentProgram() override
    {
        return 0;
    }
    void setCurrentProgram(int) override {}
    const juce::String getProgramName(int) override
    {
        return "Default";
    }
    void changeProgramName(int, const juce::String &) override {}
    void getStateInformation(juce::MemoryBlock &) override;
    void setStateInformation(const void *, int) override;
    juce::AudioProcessorParameter *getBypassParameter() const override
    {
        return state.getParameter("bypass");
    }
    bool savePreset(const juce::File &);
    bool loadPreset(const juce::File &);
    std::atomic<float> liveCutoff[2]{{120}, {8400}};
    std::atomic<double> livePhase[2]{{0}, {0}};
    std::atomic<double> liveRate{44100};

  private:
    struct Pointers
    {
        std::array<std::atomic<float> *, 15> p;
    };
    Pointers params[2];
    std::array<std::atomic<float> *, 4> masters;
    cobalt::Settings settings(int) const;
    cobalt::Engine engine;
    bool wasPlaying = false;
    double lastPpq = 0;
    void process(juce::AudioBuffer<float> &, bool);
};
