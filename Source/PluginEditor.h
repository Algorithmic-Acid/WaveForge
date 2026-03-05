#pragma once

#include <juce_gui_extra/juce_gui_extra.h>
#include "PluginProcessor.h"

//==============================================================================
class WaveForgeAudioProcessorEditor : public juce::AudioProcessorEditor,
                                       public juce::Timer
{
public:
    WaveForgeAudioProcessorEditor(WaveForgeAudioProcessor&);
    ~WaveForgeAudioProcessorEditor() override;

    void paint(juce::Graphics&) override;
    void resized() override;
    void timerCallback() override;

private:
    WaveForgeAudioProcessor& audioProcessor;
    juce::LookAndFeel_V4 lookAndFeel;

    // Oscillator controls
    juce::Slider osc1PosSlider, osc1LevelSlider;
    juce::Slider osc2PosSlider, osc2LevelSlider, osc2DetuneSlider;
    juce::Slider oscMixSlider;

    // Filter controls
    juce::Slider filterCutoffSlider, filterResSlider, filterEnvAmtSlider;

    // Amp ADSR
    juce::Slider ampAttackSlider, ampDecaySlider, ampSustainSlider, ampReleaseSlider;

    // Filter ADSR
    juce::Slider filterAttackSlider, filterDecaySlider, filterSustainSlider, filterReleaseSlider;

    // Effects
    juce::Slider reverbMixSlider, reverbSizeSlider;
    juce::Slider chorusRateSlider, chorusDepthSlider;

    // Master
    juce::Slider masterGainSlider;

    // Labels
    juce::Label titleLabel;
    juce::Label osc1Label, osc2Label, filterLabel, ampEnvLabel, filterEnvLabel, fxLabel, masterLabel;
    juce::Label presetLabel;

    // Preset UI
    juce::ComboBox   presetCombo;
    juce::TextButton savePresetButton   { "SAVE" };
    juce::TextButton deletePresetButton { "DEL" };

    // Inline save-name row (shown only while naming a new preset)
    juce::TextEditor presetNameEditor;
    juce::TextButton confirmSaveButton  { "OK" };
    juce::TextButton cancelSaveButton   { "X" };
    bool namingPreset = false;

    // Wavetable names for display
    const juce::StringArray wavetableNames = { "Sine", "Triangle", "Saw", "Square", "Pulse25", "Pulse12", "SuperSaw", "Noise" };

    // Attachments
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc1PosAtt, osc1LevelAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> osc2PosAtt, osc2LevelAtt, osc2DetuneAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> oscMixAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterCutoffAtt, filterResAtt, filterEnvAmtAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> ampAttackAtt, ampDecayAtt, ampSustainAtt, ampReleaseAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> filterAttackAtt, filterDecayAtt, filterSustainAtt, filterReleaseAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> reverbMixAtt, reverbSizeAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> chorusRateAtt, chorusDepthAtt;
    std::unique_ptr<juce::AudioProcessorValueTreeState::SliderAttachment> masterGainAtt;

    void setupSlider(juce::Slider& slider, const juce::String& suffix = "");
    void setupVerticalSlider(juce::Slider& slider);
    void drawWavetableDisplay(juce::Graphics& g, juce::Rectangle<int> bounds, float position);
    void drawEnvelopeDisplay(juce::Graphics& g, juce::Rectangle<int> bounds,
                             float a, float d, float s, float r, juce::Colour color);
    void rebuildPresetCombo();
    void onSavePreset();
    void commitSavePreset();
    void hideSaveNameEditor();
    void onDeletePreset();

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveForgeAudioProcessorEditor)
};
