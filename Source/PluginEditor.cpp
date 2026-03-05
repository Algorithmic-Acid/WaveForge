#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
WaveForgeAudioProcessorEditor::WaveForgeAudioProcessorEditor(WaveForgeAudioProcessor& p)
    : AudioProcessorEditor(&p), audioProcessor(p)
{
    // Cyan/purple synthwave color scheme
    lookAndFeel.setColour(juce::Slider::thumbColourId,               juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::Slider::rotarySliderFillColourId,    juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::Slider::rotarySliderOutlineColourId, juce::Colour(0xFF1A1A2E));
    lookAndFeel.setColour(juce::Slider::trackColourId,               juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::Slider::backgroundColourId,          juce::Colour(0xFF1A1A2E));
    lookAndFeel.setColour(juce::Slider::textBoxTextColourId,         juce::Colour(0xFFAAFFFF));
    lookAndFeel.setColour(juce::Slider::textBoxBackgroundColourId,   juce::Colour(0xFF0A0A15));
    lookAndFeel.setColour(juce::Slider::textBoxOutlineColourId,      juce::Colour(0xFF2A2A4E));
    lookAndFeel.setColour(juce::Label::textColourId,                 juce::Colour(0xFF88AACC));
    lookAndFeel.setColour(juce::ComboBox::backgroundColourId,        juce::Colour(0xFF0A0A20));
    lookAndFeel.setColour(juce::ComboBox::textColourId,              juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::ComboBox::outlineColourId,           juce::Colour(0xFF2A2A6E));
    lookAndFeel.setColour(juce::ComboBox::arrowColourId,             juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::PopupMenu::backgroundColourId,       juce::Colour(0xFF0A0A20));
    lookAndFeel.setColour(juce::PopupMenu::textColourId,             juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::PopupMenu::highlightedBackgroundColourId, juce::Colour(0xFF1A1A5E));
    lookAndFeel.setColour(juce::TextButton::buttonColourId,          juce::Colour(0xFF1A1A4E));
    lookAndFeel.setColour(juce::TextButton::textColourOffId,         juce::Colour(0xFF00FFFF));
    lookAndFeel.setColour(juce::TextButton::textColourOnId,          juce::Colour(0xFFFFFFFF));
    setLookAndFeel(&lookAndFeel);

    // Title
    titleLabel.setText("WAVEFORGE", juce::dontSendNotification);
    titleLabel.setFont(juce::Font(juce::FontOptions().withName(juce::Font::getDefaultMonospacedFontName()).withHeight(32.0f).withStyle("Bold")));
    titleLabel.setColour(juce::Label::textColourId, juce::Colour(0xFF00FFFF));
    titleLabel.setJustificationType(juce::Justification::centred);
    addAndMakeVisible(titleLabel);

    // Section labels
    auto setupLabel = [this](juce::Label& label, const juce::String& text) {
        label.setText(text, juce::dontSendNotification);
        label.setFont(juce::Font(juce::FontOptions().withName(juce::Font::getDefaultMonospacedFontName()).withHeight(11.0f).withStyle("Bold")));
        label.setColour(juce::Label::textColourId, juce::Colour(0xFFFF00FF));
        addAndMakeVisible(label);
    };

    setupLabel(osc1Label,     "OSC 1");
    setupLabel(osc2Label,     "OSC 2");
    setupLabel(filterLabel,   "FILTER");
    setupLabel(ampEnvLabel,   "AMP ENV");
    setupLabel(filterEnvLabel,"FILTER ENV");
    setupLabel(fxLabel,       "FX");
    setupLabel(masterLabel,   "MASTER");
    setupLabel(presetLabel,   "PRESETS");

    // Setup sliders
    setupSlider(osc1PosSlider);   setupSlider(osc1LevelSlider);
    setupSlider(osc2PosSlider);   setupSlider(osc2LevelSlider);   setupSlider(osc2DetuneSlider, " ct");
    setupSlider(oscMixSlider);
    setupSlider(filterCutoffSlider, " Hz"); setupSlider(filterResSlider); setupSlider(filterEnvAmtSlider);

    setupVerticalSlider(ampAttackSlider);    setupVerticalSlider(ampDecaySlider);
    setupVerticalSlider(ampSustainSlider);   setupVerticalSlider(ampReleaseSlider);
    setupVerticalSlider(filterAttackSlider); setupVerticalSlider(filterDecaySlider);
    setupVerticalSlider(filterSustainSlider);setupVerticalSlider(filterReleaseSlider);

    setupSlider(reverbMixSlider); setupSlider(reverbSizeSlider);
    setupSlider(chorusRateSlider, " Hz"); setupSlider(chorusDepthSlider);
    setupSlider(masterGainSlider, " dB");

    // Attachments
    osc1PosAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "osc1Pos",     osc1PosSlider);
    osc1LevelAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "osc1Level",   osc1LevelSlider);
    osc2PosAtt      = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "osc2Pos",     osc2PosSlider);
    osc2LevelAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "osc2Level",   osc2LevelSlider);
    osc2DetuneAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "osc2Detune",  osc2DetuneSlider);
    oscMixAtt       = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "oscMix",      oscMixSlider);
    filterCutoffAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterCutoff",filterCutoffSlider);
    filterResAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterRes",   filterResSlider);
    filterEnvAmtAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterEnvAmt",filterEnvAmtSlider);
    ampAttackAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ampAttack",   ampAttackSlider);
    ampDecayAtt     = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ampDecay",    ampDecaySlider);
    ampSustainAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ampSustain",  ampSustainSlider);
    ampReleaseAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "ampRelease",  ampReleaseSlider);
    filterAttackAtt = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterAttack",filterAttackSlider);
    filterDecayAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterDecay", filterDecaySlider);
    filterSustainAtt= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterSustain",filterSustainSlider);
    filterReleaseAtt= std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "filterRelease",filterReleaseSlider);
    reverbMixAtt    = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "reverbMix",  reverbMixSlider);
    reverbSizeAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "reverbSize", reverbSizeSlider);
    chorusRateAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "chorusRate", chorusRateSlider);
    chorusDepthAtt  = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "chorusDepth",chorusDepthSlider);
    masterGainAtt   = std::make_unique<juce::AudioProcessorValueTreeState::SliderAttachment>(audioProcessor.apvts, "masterGain", masterGainSlider);

    // Preset combo box
    presetCombo.setJustificationType(juce::Justification::centredLeft);
    presetCombo.onChange = [this] {
        int idx = presetCombo.getSelectedItemIndex();
        if (idx >= 0)
            audioProcessor.applyPreset(idx);
    };
    addAndMakeVisible(presetCombo);

    // Save preset button — shows the inline name editor
    savePresetButton.onClick = [this] { onSavePreset(); };
    addAndMakeVisible(savePresetButton);

    // Delete preset button (only enabled for user presets)
    deletePresetButton.onClick = [this] { onDeletePreset(); };
    addAndMakeVisible(deletePresetButton);

    // Inline name editor (hidden until SAVE is clicked)
    presetNameEditor.setTextToShowWhenEmpty("Preset name...", juce::Colours::grey);
    presetNameEditor.setColour(juce::TextEditor::backgroundColourId, juce::Colour(0xFF0A0A20));
    presetNameEditor.setColour(juce::TextEditor::textColourId,       juce::Colour(0xFF00FFFF));
    presetNameEditor.setColour(juce::TextEditor::outlineColourId,    juce::Colour(0xFF2A2A6E));
    presetNameEditor.onReturnKey = [this] { commitSavePreset(); };
    presetNameEditor.onEscapeKey = [this] { hideSaveNameEditor(); };
    addChildComponent(presetNameEditor);

    confirmSaveButton.onClick = [this] { commitSavePreset(); };
    addChildComponent(confirmSaveButton);

    cancelSaveButton.onClick = [this] { hideSaveNameEditor(); };
    addChildComponent(cancelSaveButton);

    rebuildPresetCombo();

    setSize(950, 440);
    startTimerHz(30);
}

WaveForgeAudioProcessorEditor::~WaveForgeAudioProcessorEditor()
{
    stopTimer();
    setLookAndFeel(nullptr);
}

//==============================================================================
void WaveForgeAudioProcessorEditor::setupSlider(juce::Slider& slider, const juce::String& suffix)
{
    slider.setSliderStyle(juce::Slider::RotaryHorizontalVerticalDrag);
    slider.setTextBoxStyle(juce::Slider::TextBoxBelow, false, 55, 14);
    slider.setTextValueSuffix(suffix);
    addAndMakeVisible(slider);
}

void WaveForgeAudioProcessorEditor::setupVerticalSlider(juce::Slider& slider)
{
    slider.setSliderStyle(juce::Slider::LinearVertical);
    slider.setTextBoxStyle(juce::Slider::NoTextBox, false, 0, 0);
    addAndMakeVisible(slider);
}

//==============================================================================
void WaveForgeAudioProcessorEditor::rebuildPresetCombo()
{
    presetCombo.clear(juce::dontSendNotification);

    // Built-in presets
    presetCombo.addSectionHeading("Factory Presets");
    for (int i = 0; i < NUM_BUILTIN_PRESETS; ++i)
        presetCombo.addItem(WaveForgeAudioProcessor::builtInPresets[i].name, i + 1);

    // User presets
    const auto& userPresets = audioProcessor.getUserPresets();
    if (!userPresets.empty())
    {
        presetCombo.addSeparator();
        presetCombo.addSectionHeading("My Presets");
        for (int i = 0; i < (int)userPresets.size(); ++i)
            presetCombo.addItem(userPresets[i].name, NUM_BUILTIN_PRESETS + i + 1);
    }

    // Reflect current program
    int cur = audioProcessor.getCurrentProgram();
    presetCombo.setSelectedItemIndex(cur, juce::dontSendNotification);

    // Delete button only active for user presets
    deletePresetButton.setEnabled(cur >= NUM_BUILTIN_PRESETS);
}

void WaveForgeAudioProcessorEditor::onSavePreset()
{
    // Show the inline name editor
    namingPreset = true;
    presetNameEditor.setText("My Preset", false);
    presetNameEditor.setVisible(true);
    presetNameEditor.grabKeyboardFocus();
    presetNameEditor.selectAll();
    confirmSaveButton.setVisible(true);
    cancelSaveButton.setVisible(true);
    savePresetButton.setVisible(false);
    resized(); // re-layout
}

void WaveForgeAudioProcessorEditor::commitSavePreset()
{
    auto name = presetNameEditor.getText().trim();
    if (name.isNotEmpty())
    {
        audioProcessor.saveUserPreset(name);
        rebuildPresetCombo();
        int idx = NUM_BUILTIN_PRESETS + (int)audioProcessor.getUserPresets().size() - 1;
        presetCombo.setSelectedItemIndex(idx, juce::dontSendNotification);
    }
    hideSaveNameEditor();
}

void WaveForgeAudioProcessorEditor::hideSaveNameEditor()
{
    namingPreset = false;
    presetNameEditor.setVisible(false);
    confirmSaveButton.setVisible(false);
    cancelSaveButton.setVisible(false);
    savePresetButton.setVisible(true);
    resized();
}

void WaveForgeAudioProcessorEditor::onDeletePreset()
{
    int cur = audioProcessor.getCurrentProgram();
    int userIdx = cur - NUM_BUILTIN_PRESETS;
    if (userIdx < 0 || userIdx >= (int)audioProcessor.getUserPresets().size())
        return;

    auto presetName = audioProcessor.getUserPresets()[userIdx].name;

    // JUCE 8: async — no modal loops
    juce::AlertWindow::showAsync(
        juce::MessageBoxOptions()
            .withIconType(juce::MessageBoxIconType::WarningIcon)
            .withTitle("Delete Preset")
            .withMessage("Delete \"" + presetName + "\"?")
            .withButton("Delete")
            .withButton("Cancel"),
        [this, userIdx](int result)
        {
            if (result == 1) // "Delete" button
            {
                audioProcessor.getUserPresets()[userIdx].file.deleteFile();
                audioProcessor.loadUserPresetsFromDisk();
                audioProcessor.applyPreset(0);
                rebuildPresetCombo();
            }
        });
}

//==============================================================================
void WaveForgeAudioProcessorEditor::timerCallback()
{
    repaint();

    // Keep combo in sync if the host changed the program
    int hostProg = audioProcessor.getCurrentProgram();
    if (presetCombo.getSelectedItemIndex() != hostProg)
        presetCombo.setSelectedItemIndex(hostProg, juce::dontSendNotification);

    // Enable/disable delete button
    deletePresetButton.setEnabled(hostProg >= NUM_BUILTIN_PRESETS);
}

//==============================================================================
void WaveForgeAudioProcessorEditor::drawWavetableDisplay(juce::Graphics& g, juce::Rectangle<int> bounds, float position)
{
    g.setColour(juce::Colour(0xFF0A0A15));
    g.fillRoundedRectangle(bounds.toFloat(), 4);
    g.setColour(juce::Colour(0xFF2A2A4E));
    g.drawRoundedRectangle(bounds.toFloat(), 4, 1);

    int tableIndex = juce::jlimit(0, 7, static_cast<int>(position));
    const float* table = audioProcessor.getWavetable(tableIndex);

    juce::Path wavePath;
    float centerY  = bounds.getCentreY();
    float amplitude = bounds.getHeight() * 0.35f;

    int step = WaveForgeAudioProcessor::getWavetableSize() / bounds.getWidth();
    step = juce::jmax(1, step);

    for (int x = 0; x < bounds.getWidth(); ++x)
    {
        int tableIdx = (x * step) % WaveForgeAudioProcessor::getWavetableSize();
        float y = centerY - table[tableIdx] * amplitude;
        if (x == 0) wavePath.startNewSubPath(bounds.getX() + x, y);
        else         wavePath.lineTo         (bounds.getX() + x, y);
    }

    g.setColour(juce::Colour(0xFF00FFFF));
    g.strokePath(wavePath, juce::PathStrokeType(1.5f));

    g.setFont(juce::Font(juce::FontOptions().withName(juce::Font::getDefaultMonospacedFontName()).withHeight(10.0f)));
    g.drawText(wavetableNames[tableIndex], bounds.removeFromBottom(14), juce::Justification::centred);
}

void WaveForgeAudioProcessorEditor::drawEnvelopeDisplay(juce::Graphics& g, juce::Rectangle<int> bounds,
                                                         float a, float d, float s, float r, juce::Colour color)
{
    g.setColour(juce::Colour(0xFF0A0A15));
    g.fillRoundedRectangle(bounds.toFloat(), 4);
    g.setColour(juce::Colour(0xFF2A2A4E));
    g.drawRoundedRectangle(bounds.toFloat(), 4, 1);

    float maxTime = 2.0f;
    a = juce::jmin(a, maxTime);
    d = juce::jmin(d, maxTime);
    r = juce::jmin(r, maxTime);
    float totalTime = a + d + 0.3f + r;
    float scaleX = (bounds.getWidth() - 4) / totalTime;

    juce::Path envPath;
    float x      = bounds.getX() + 2.0f;
    float bottom = bounds.getBottom() - 2.0f;
    float top    = bounds.getY() + 2.0f;
    float height = bottom - top;

    envPath.startNewSubPath(x, bottom);
    x += a * scaleX; envPath.lineTo(x, top);
    x += d * scaleX;
    float sustainY = bottom - s * height;
    envPath.lineTo(x, sustainY);
    x += 0.3f * scaleX; envPath.lineTo(x, sustainY);
    x += r * scaleX;    envPath.lineTo(x, bottom);

    g.setColour(color.withAlpha(0.3f));
    juce::Path filled = envPath;
    filled.lineTo(bounds.getRight() - 2.0f, bottom);
    filled.closeSubPath();
    g.fillPath(filled);

    g.setColour(color);
    g.strokePath(envPath, juce::PathStrokeType(1.5f));
}

//==============================================================================
void WaveForgeAudioProcessorEditor::paint(juce::Graphics& g)
{
    // Background gradient
    juce::ColourGradient bg(juce::Colour(0xFF0A0A15), 0, 0,
                            juce::Colour(0xFF151528), 0, static_cast<float>(getHeight()), false);
    g.setGradientFill(bg);
    g.fillAll();

    // Subtle grid
    g.setColour(juce::Colour(0x10FFFFFF));
    for (int x = 0; x < getWidth();  x += 50) g.drawVerticalLine  (x, 0, static_cast<float>(getHeight()));
    for (int y = 0; y < getHeight(); y += 50) g.drawHorizontalLine(y, 0, static_cast<float>(getWidth()));

    // Section backgrounds
    auto drawSection = [&](int x, int y, int w, int h) {
        g.setColour(juce::Colour(0x15FFFFFF));
        g.fillRoundedRectangle(static_cast<float>(x), static_cast<float>(y),
                               static_cast<float>(w), static_cast<float>(h), 6);
    };

    drawSection(15,  55, 200, 180);   // OSC 1
    drawSection(225, 55, 200, 180);   // OSC 2
    drawSection(435, 55, 170, 180);   // Filter
    drawSection(615, 55, 160, 180);   // Amp Env
    drawSection(785, 55, 150, 180);   // Filter Env
    drawSection(15,  250, 300, 160);  // FX
    drawSection(325, 250, 100, 160);  // Master
    drawSection(435, 250, 490, 160);  // Presets

    // Title glow
    g.setColour(juce::Colour(0x2000FFFF));
    g.fillRect(0, 0, getWidth(), 50);

    // Wavetable displays
    float osc1Pos = audioProcessor.apvts.getRawParameterValue("osc1Pos")->load();
    float osc2Pos = audioProcessor.apvts.getRawParameterValue("osc2Pos")->load();
    drawWavetableDisplay(g, juce::Rectangle<int>(25,  75, 180, 60), osc1Pos);
    drawWavetableDisplay(g, juce::Rectangle<int>(235, 75, 180, 60), osc2Pos);

    // Envelope displays
    float ampA = audioProcessor.apvts.getRawParameterValue("ampAttack")->load();
    float ampD = audioProcessor.apvts.getRawParameterValue("ampDecay")->load();
    float ampS = audioProcessor.apvts.getRawParameterValue("ampSustain")->load();
    float ampR = audioProcessor.apvts.getRawParameterValue("ampRelease")->load();
    drawEnvelopeDisplay(g, juce::Rectangle<int>(625, 75, 140, 50), ampA, ampD, ampS, ampR, juce::Colour(0xFF00FFFF));

    float fltA = audioProcessor.apvts.getRawParameterValue("filterAttack")->load();
    float fltD = audioProcessor.apvts.getRawParameterValue("filterDecay")->load();
    float fltS = audioProcessor.apvts.getRawParameterValue("filterSustain")->load();
    float fltR = audioProcessor.apvts.getRawParameterValue("filterRelease")->load();
    drawEnvelopeDisplay(g, juce::Rectangle<int>(795, 75, 130, 50), fltA, fltD, fltS, fltR, juce::Colour(0xFFFF00FF));

    // Knob / slider labels
    g.setColour(juce::Colour(0xFF667788));
    g.setFont(juce::Font(juce::FontOptions().withName(juce::Font::getDefaultMonospacedFontName()).withHeight(9.0f)));

    g.drawText("WAVE",   35,  195, 50, 12, juce::Justification::centred);
    g.drawText("LEVEL",  105, 195, 50, 12, juce::Justification::centred);
    g.drawText("MIX",    170, 195, 50, 12, juce::Justification::centred);
    g.drawText("WAVE",   245, 195, 50, 12, juce::Justification::centred);
    g.drawText("LEVEL",  305, 195, 50, 12, juce::Justification::centred);
    g.drawText("DETUNE", 360, 195, 55, 12, juce::Justification::centred);

    g.drawText("CUTOFF", 445, 195, 50, 12, juce::Justification::centred);
    g.drawText("RES",    505, 195, 50, 12, juce::Justification::centred);
    g.drawText("ENV",    555, 195, 50, 12, juce::Justification::centred);

    g.drawText("A",  625, 215, 30, 12, juce::Justification::centred);
    g.drawText("D",  660, 215, 30, 12, juce::Justification::centred);
    g.drawText("S",  695, 215, 30, 12, juce::Justification::centred);
    g.drawText("R",  730, 215, 30, 12, juce::Justification::centred);
    g.drawText("A",  795, 215, 30, 12, juce::Justification::centred);
    g.drawText("D",  825, 215, 30, 12, juce::Justification::centred);
    g.drawText("S",  855, 215, 30, 12, juce::Justification::centred);
    g.drawText("R",  885, 215, 30, 12, juce::Justification::centred);

    g.drawText("REV MIX",  30,  370, 55, 12, juce::Justification::centred);
    g.drawText("REV SIZE", 95,  370, 55, 12, juce::Justification::centred);
    g.drawText("CHO RATE", 165, 370, 55, 12, juce::Justification::centred);
    g.drawText("CHO DPTH", 235, 370, 55, 12, juce::Justification::centred);
    g.drawText("GAIN",     345, 370, 60, 12, juce::Justification::centred);

    // Border
    g.setColour(juce::Colour(0xFF2A2A4E));
    g.drawRect(getLocalBounds(), 2);
}

//==============================================================================
void WaveForgeAudioProcessorEditor::resized()
{
    titleLabel.setBounds(0, 5, getWidth(), 40);

    // Section labels
    osc1Label.setBounds    (20,  58, 60, 15);
    osc2Label.setBounds    (230, 58, 60, 15);
    filterLabel.setBounds  (440, 58, 60, 15);
    ampEnvLabel.setBounds  (620, 58, 80, 15);
    filterEnvLabel.setBounds(790,58, 80, 15);
    fxLabel.setBounds      (20,  253, 30, 15);
    masterLabel.setBounds  (330, 253, 60, 15);
    presetLabel.setBounds  (440, 253, 70, 15);

    const int knobSize  = 55;
    const int smallKnob = 50;

    // OSC 1
    osc1PosSlider.setBounds   (30,  140, knobSize, knobSize);
    osc1LevelSlider.setBounds (100, 140, knobSize, knobSize);

    // OSC Mix (sits between sections)
    oscMixSlider.setBounds(170, 140, smallKnob, smallKnob);

    // OSC 2
    osc2PosSlider.setBounds   (240, 140, knobSize, knobSize);
    osc2LevelSlider.setBounds (300, 140, knobSize, knobSize);
    osc2DetuneSlider.setBounds(360, 140, knobSize, knobSize);

    // Filter
    filterCutoffSlider.setBounds(445, 140, smallKnob, smallKnob);
    filterResSlider.setBounds   (500, 140, smallKnob, smallKnob);
    filterEnvAmtSlider.setBounds(555, 140, smallKnob, smallKnob);

    // Amp ADSR (vertical sliders)
    const int envSliderH = 80, envSliderW = 25, envY = 130;
    ampAttackSlider.setBounds  (625, envY, envSliderW, envSliderH);
    ampDecaySlider.setBounds   (660, envY, envSliderW, envSliderH);
    ampSustainSlider.setBounds (695, envY, envSliderW, envSliderH);
    ampReleaseSlider.setBounds (730, envY, envSliderW, envSliderH);

    // Filter ADSR
    filterAttackSlider.setBounds (795, envY, envSliderW, envSliderH);
    filterDecaySlider.setBounds  (825, envY, envSliderW, envSliderH);
    filterSustainSlider.setBounds(855, envY, envSliderW, envSliderH);
    filterReleaseSlider.setBounds(885, envY, envSliderW, envSliderH);

    // FX
    const int fxY = 280;
    reverbMixSlider.setBounds  (25,  fxY, knobSize, knobSize);
    reverbSizeSlider.setBounds (90,  fxY, knobSize, knobSize);
    chorusRateSlider.setBounds (165, fxY, knobSize, knobSize);
    chorusDepthSlider.setBounds(235, fxY, knobSize, knobSize);

    // Master
    masterGainSlider.setBounds(345, fxY, knobSize, knobSize);

    // Presets section (right side of bottom row)
    presetCombo.setBounds        (445, 280, 320, 28);
    deletePresetButton.setBounds (775, 280,  35, 28);

    if (namingPreset)
    {
        // Inline name editor row
        savePresetButton.setBounds   (  0,   0,  0,  0); // hidden
        presetNameEditor.setBounds   (445, 318, 300, 26);
        confirmSaveButton.setBounds  (753, 318,  30, 26);
        cancelSaveButton.setBounds   (789, 318,  21, 26);
    }
    else
    {
        savePresetButton.setBounds   (818, 280,  55, 28);
        presetNameEditor.setBounds   (  0,   0,  0,  0);
        confirmSaveButton.setBounds  (  0,   0,  0,  0);
        cancelSaveButton.setBounds   (  0,   0,  0,  0);
    }
}
