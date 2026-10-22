#pragma once
#include "PluginProcessor.h"
#include <map>
class CobaltLook : public juce::LookAndFeel_V4
{
  public:
    CobaltLook();
    juce::Typeface::Ptr getTypefaceForFont(const juce::Font &) override;
    juce::Font getTextButtonFont(juce::TextButton &, int) override;
    juce::Font getComboBoxFont(juce::ComboBox &) override;
    juce::Font getPopupMenuFont() override;
    juce::Label *createSliderTextBox(juce::Slider &) override;
    void drawLabel(juce::Graphics &, juce::Label &) override;
    void drawToggleButton(juce::Graphics &, juce::ToggleButton &, bool, bool) override;
    void drawButtonBackground(juce::Graphics &, juce::Button &, const juce::Colour &, bool,
                              bool) override;
    void drawComboBox(juce::Graphics &, int, int, bool, int, int, int, int,
                      juce::ComboBox &) override;
    void drawLinearSlider(juce::Graphics &, int, int, int, int, float, float, float,
                          juce::Slider::SliderStyle, juce::Slider &) override;
    void drawTooltip(juce::Graphics &, const juce::String &, int, int) override;
    void drawRotarySlider(juce::Graphics &, int, int, int, int, float, float, float,
                          juce::Slider &) override;

  private:
    std::map<int, juce::Image> dialImages;
    const juce::Image &dialBase(int);
};
class FilterPanel : public juce::Component
{
  public:
    FilterPanel(CobaltProcessor &, int);
    void paint(juce::Graphics &) override;
    void resized() override;
    void animate();

  private:
    CobaltProcessor &processor;
    int index;
    juce::Colour accent;
    juce::String prefix;
    std::vector<std::unique_ptr<juce::Slider>> sliders;
    std::vector<std::unique_ptr<juce::Label>> labels;
    std::vector<std::unique_ptr<juce::ComboBox>> combos;
    std::vector<std::unique_ptr<juce::ToggleButton>> toggles;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment>> sa;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ComboBoxAttachment>> ca;
    std::vector<std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment>> ba;
    void addSlider(const char *, const char *);
    void addCombo(const char *, juce::StringArray);
    void addToggle(const char *, const char *);
};
class ResponseGraph : public juce::Component
{
  public:
    explicit ResponseGraph(CobaltProcessor &p) : processor(p) {}
    void paint(juce::Graphics &) override;
    void mouseDown(const juce::MouseEvent &) override;
    void mouseDrag(const juce::MouseEvent &) override;
    void mouseUp(const juce::MouseEvent &) override;

  private:
    CobaltProcessor &processor;
    int selected = -1;
    juce::Rectangle<float> plot() const;
    float xFor(double) const;
    float yFor(double) const;
};
class CobaltContent : public juce::Component, private juce::Timer
{
  public:
    explicit CobaltContent(CobaltProcessor &);
    ~CobaltContent() override;
    void paint(juce::Graphics &) override;
    void resized() override;
    void updateScale(float);
    std::function<void(float)> onScaleChange;

  private:
    CobaltProcessor &processor;
    juce::ComboBox uiSize;
    CobaltLook look;
    juce::Image brandLogo;
    juce::TooltipWindow tooltips{this, 700};
    ResponseGraph graph;
    FilterPanel hp, lp;
    juce::Slider input, output, mix;
    juce::Label inputLabel, outputLabel, mixLabel;
    juce::ToggleButton bypass{"BYPASS"};
    juce::TextButton save{"SAVE"}, load{"LOAD"};
    std::unique_ptr<juce::FileChooser> chooser;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ia, oa, ma;
    std::unique_ptr<juce::AudioProcessorValueTreeState::ButtonAttachment> ba;
    void timerCallback() override;
    void preset(bool);
};

// Keep the design in a fixed coordinate space so text, hit targets and all
// controls scale together instead of clipping when the host window shrinks.
class CobaltEditor : public juce::AudioProcessorEditor
{
  public:
    explicit CobaltEditor(CobaltProcessor &);
    void resized() override;
    void paint(juce::Graphics &) override;

  private:
    CobaltProcessor &processor;
    CobaltContent content;
};
