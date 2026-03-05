#include "PluginProcessor.h"
#include "PluginEditor.h"

//==============================================================================
// Built-in Presets
// Values follow the PARAM_IDS order:
// osc1Pos, osc1Level, osc2Pos, osc2Level, osc2Detune, oscMix,
// filterCutoff, filterRes, filterEnvAmt,
// ampAttack, ampDecay, ampSustain, ampRelease,
// filterAttack, filterDecay, filterSustain, filterRelease,
// reverbMix, reverbSize, chorusRate, chorusDepth, masterGain
//==============================================================================
const std::array<Preset, NUM_BUILTIN_PRESETS> WaveForgeAudioProcessor::builtInPresets = {{
    { "Init",        { 0.0f, 1.0f, 2.0f, 0.5f,   5.0f, 0.5f,  5000.0f, 0.30f,  0.5f, 0.010f, 0.10f, 0.80f, 0.30f, 0.010f, 0.30f, 0.20f, 0.50f, 0.20f, 0.50f, 1.0f, 0.00f,  0.0f } },
    { "Warm Pad",    { 0.0f, 0.8f, 1.0f, 0.6f,   7.0f, 0.5f,  2000.0f, 0.40f,  0.7f, 1.500f, 0.80f, 0.70f, 2.50f, 0.800f, 0.50f, 0.40f, 2.00f, 0.40f, 0.70f, 0.8f, 0.20f, -2.0f } },
    { "Deep Bass",   { 2.0f, 1.0f, 3.0f, 0.7f, -12.0f, 0.3f,   600.0f, 0.50f,  0.8f, 0.005f, 0.20f, 0.90f, 0.20f, 0.010f, 0.15f, 0.00f, 0.20f, 0.00f, 0.30f, 1.0f, 0.00f,  2.0f } },
    { "Synth Lead",  { 2.0f, 1.0f, 2.0f, 0.4f,   3.0f, 0.3f,  8000.0f, 0.30f,  0.4f, 0.010f, 0.10f, 0.60f, 0.10f, 0.010f, 0.10f, 0.20f, 0.30f, 0.15f, 0.40f, 2.0f, 0.20f,  0.0f } },
    { "Pluck",       { 2.0f, 1.0f, 3.0f, 0.5f,   5.0f, 0.5f, 15000.0f, 0.20f,  0.8f, 0.002f, 0.30f, 0.00f, 0.50f, 0.002f, 0.20f, 0.00f, 0.50f, 0.20f, 0.40f, 0.5f, 0.10f,  0.0f } },
    { "Bell",        { 0.0f, 1.0f, 0.0f, 0.4f,   7.0f, 0.4f, 12000.0f, 0.10f,  0.3f, 0.005f, 2.00f, 0.00f, 3.00f, 0.005f, 1.00f, 0.00f, 2.00f, 0.35f, 0.70f, 0.5f, 0.10f, -3.0f } },
    { "Brass",       { 2.0f, 1.0f, 3.0f, 0.6f,   0.0f, 0.5f,  5000.0f, 0.50f,  0.7f, 0.050f, 0.15f, 0.80f, 0.30f, 0.050f, 0.20f, 0.50f, 0.30f, 0.10f, 0.40f, 1.0f, 0.10f,  0.0f } },
    { "Ambient",     { 6.0f, 0.9f, 6.0f, 0.7f,  15.0f, 0.5f,  3000.0f, 0.30f,  0.5f, 2.000f, 1.00f, 0.60f, 4.00f, 1.000f, 0.80f, 0.40f, 3.00f, 0.50f, 0.80f, 0.3f, 0.30f, -4.0f } },
}};

//==============================================================================
// ADSREnvelope Implementation
//==============================================================================
void ADSREnvelope::setParameters(float attack, float decay, float sustain, float release, float sampleRate)
{
    attackRate  = 1.0f / (juce::jmax(0.001f, attack)  * sampleRate);
    decayRate   = 1.0f / (juce::jmax(0.001f, decay)   * sampleRate);
    sustainLevel = sustain;
    releaseRate = 1.0f / (juce::jmax(0.001f, release) * sampleRate);
}

void ADSREnvelope::noteOn()
{
    stage = Stage::Attack;
}

void ADSREnvelope::noteOff()
{
    if (stage != Stage::Idle)
        stage = Stage::Release;
}

float ADSREnvelope::getNextValue()
{
    switch (stage)
    {
        case Stage::Idle:
            return 0.0f;

        case Stage::Attack:
            currentValue += attackRate;
            if (currentValue >= 1.0f)
            {
                currentValue = 1.0f;
                stage = Stage::Decay;
            }
            break;

        case Stage::Decay:
            currentValue -= decayRate;
            if (currentValue <= sustainLevel)
            {
                currentValue = sustainLevel;
                stage = Stage::Sustain;
            }
            break;

        case Stage::Sustain:
            currentValue = sustainLevel;
            break;

        case Stage::Release:
            currentValue -= releaseRate;
            if (currentValue <= 0.0f)
            {
                currentValue = 0.0f;
                stage = Stage::Idle;
            }
            break;
    }

    return currentValue;
}

//==============================================================================
// WavetableOscillator Implementation
//==============================================================================
void WavetableOscillator::prepare(double sr)
{
    sampleRate = sr;
    phase = 0.0f;
}

void WavetableOscillator::setWavetableBank(const std::array<std::array<float, WAVETABLE_SIZE>, NUM_WAVETABLES>& bank)
{
    wavetableBank = &bank;
}

void WavetableOscillator::setFrequency(float freq)
{
    phaseIncrement = freq / static_cast<float>(sampleRate);
}

void WavetableOscillator::setWavetablePosition(float position)
{
    tablePosition = juce::jlimit(0.0f, static_cast<float>(NUM_WAVETABLES - 1), position);
}

float WavetableOscillator::getNextSample()
{
    if (wavetableBank == nullptr)
        return 0.0f;

    // Get interpolated position in table
    float tableIndex = phase * WAVETABLE_SIZE;
    int index0 = static_cast<int>(tableIndex);
    int index1 = (index0 + 1) % WAVETABLE_SIZE;
    float frac = tableIndex - index0;

    // Interpolate between wavetables
    int table0 = static_cast<int>(tablePosition);
    int table1 = juce::jmin(table0 + 1, NUM_WAVETABLES - 1);
    float tableFrac = tablePosition - table0;

    // Get samples from both tables
    const auto& bank = *wavetableBank;
    float sampleT0 = bank[table0][index0] + frac * (bank[table0][index1] - bank[table0][index0]);
    float sampleT1 = bank[table1][index0] + frac * (bank[table1][index1] - bank[table1][index0]);

    float sample = sampleT0 + tableFrac * (sampleT1 - sampleT0);

    // Advance phase
    phase += phaseIncrement;
    if (phase >= 1.0f) phase -= 1.0f;

    return sample;
}

//==============================================================================
// SynthVoice Implementation
//==============================================================================
void SynthVoice::prepare(double sr, int /*samplesPerBlock*/)
{
    sampleRate = sr;
    osc1.prepare(sr);
    osc2.prepare(sr);
    reset();
}

void SynthVoice::setWavetableBank(const std::array<std::array<float, WAVETABLE_SIZE>, NUM_WAVETABLES>& bank)
{
    osc1.setWavetableBank(bank);
    osc2.setWavetableBank(bank);
}

void SynthVoice::noteOn(int midiNote, float vel)
{
    currentNote = midiNote;
    velocity = vel;
    baseFrequency = 440.0f * std::pow(2.0f, (midiNote - 69) / 12.0f);

    // Clear filter state to avoid clicks on voice steal
    for (int i = 0; i < 4; ++i) filterState[i] = 0.0f;

    osc1.reset();
    osc2.reset();
    ampEnv.noteOn();
    filterEnv.noteOn();
}

void SynthVoice::noteOff()
{
    ampEnv.noteOff();
    filterEnv.noteOff();
}

void SynthVoice::reset()
{
    currentNote = -1;
    ampEnv.reset();
    filterEnv.reset();
    osc1.reset();
    osc2.reset();
    for (int i = 0; i < 4; ++i) filterState[i] = 0.0f;
}

void SynthVoice::processFilter(float& sample, float cutoff, float resonance)
{
    // State Variable Filter (2-pole)
    float f = 2.0f * std::sin(juce::MathConstants<float>::pi * cutoff / static_cast<float>(sampleRate));
    f = juce::jlimit(0.0f, 0.99f, f);
    float q = juce::jmax(0.01f, 1.0f - resonance);

    float low  = filterState[0] + f * filterState[1];
    float high = sample - low - q * filterState[1];
    float band = f * high + filterState[1];

    filterState[0] = low;
    filterState[1] = band;

    sample = low; // Lowpass output
}

void SynthVoice::renderNextBlock(float* outputL, float* outputR, int numSamples,
                                  float osc1Pos, float osc2Pos, float osc1Level, float osc2Level,
                                  float oscMix, float detune, float filterCutoff, float filterRes,
                                  float filterEnvAmt, float* ampADSR, float* filterADSR,
                                  float pan, float pitchBend)
{
    if (!isActive()) return;

    // Set envelope parameters
    ampEnv.setParameters   (ampADSR[0],    ampADSR[1],    ampADSR[2],    ampADSR[3],    static_cast<float>(sampleRate));
    filterEnv.setParameters(filterADSR[0], filterADSR[1], filterADSR[2], filterADSR[3], static_cast<float>(sampleRate));

    // Calculate pitch with bend (±2 semitones)
    float pitchMult = std::pow(2.0f, pitchBend * 2.0f / 12.0f);
    float freq1 = baseFrequency * pitchMult;
    float freq2 = baseFrequency * pitchMult * std::pow(2.0f, detune / 1200.0f);

    osc1.setWavetablePosition(osc1Pos);
    osc2.setWavetablePosition(osc2Pos);

    float panL = std::cos(pan * juce::MathConstants<float>::halfPi);
    float panR = std::sin(pan * juce::MathConstants<float>::halfPi);

    for (int i = 0; i < numSamples; ++i)
    {
        osc1.setFrequency(freq1);
        osc2.setFrequency(freq2);

        float osc1Sample = osc1.getNextSample() * osc1Level;
        float osc2Sample = osc2.getNextSample() * osc2Level;
        float sample = osc1Sample * (1.0f - oscMix) + osc2Sample * oscMix;

        // Filter with envelope modulation
        float filterEnvValue = filterEnv.getNextValue();
        float modulatedCutoff = filterCutoff + filterEnvValue * filterEnvAmt * 10000.0f;
        modulatedCutoff = juce::jlimit(20.0f, 20000.0f, modulatedCutoff);
        processFilter(sample, modulatedCutoff, filterRes);

        // Amplitude envelope + velocity
        float ampEnvValue = ampEnv.getNextValue();
        sample *= ampEnvValue * velocity;

        outputL[i] += sample * panL;
        outputR[i] += sample * panR;
    }
}

//==============================================================================
// WaveForgeAudioProcessor Implementation
//==============================================================================
WaveForgeAudioProcessor::WaveForgeAudioProcessor()
#ifndef JucePlugin_PreferredChannelConfigurations
    : AudioProcessor(BusesProperties()
        .withOutput("Output", juce::AudioChannelSet::stereo(), true)),
#endif
    apvts(*this, nullptr, "Parameters", createParameterLayout())
{
    initializeWavetables();
    loadUserPresetsFromDisk();
}

WaveForgeAudioProcessor::~WaveForgeAudioProcessor() {}

void WaveForgeAudioProcessor::initializeWavetables()
{
    for (int i = 0; i < WAVETABLE_SIZE; ++i)
    {
        float phase = static_cast<float>(i) / WAVETABLE_SIZE;
        float angle = phase * juce::MathConstants<float>::twoPi;

        // 0: Sine
        wavetables[0][i] = std::sin(angle);

        // 1: Triangle
        wavetables[1][i] = 2.0f * std::abs(2.0f * phase - 1.0f) - 1.0f;

        // 2: Saw (band-limited approximation)
        wavetables[2][i] = 0.0f;
        for (int h = 1; h <= 32; ++h)
            wavetables[2][i] += std::sin(h * angle) * (h % 2 == 0 ? -1.0f : 1.0f) / h;
        wavetables[2][i] *= 0.6f;

        // 3: Square (band-limited)
        wavetables[3][i] = 0.0f;
        for (int h = 1; h <= 32; h += 2)
            wavetables[3][i] += std::sin(h * angle) / h;
        wavetables[3][i] *= 0.8f;

        // 4: Pulse 25%
        wavetables[4][i] = phase < 0.25f ? 1.0f : -1.0f;

        // 5: Pulse 12.5%
        wavetables[5][i] = phase < 0.125f ? 1.0f : -1.0f;

        // 6: SuperSaw (detuned saws summed)
        wavetables[6][i] = 0.0f;
        for (int d = -2; d <= 2; ++d)
        {
            float detunedPhase = std::fmod(phase * (1.0f + d * 0.01f) + 0.5f, 1.0f);
            wavetables[6][i] += (2.0f * detunedPhase - 1.0f);
        }
        wavetables[6][i] *= 0.3f;

        // 7: Noise (stored random values, seeded deterministically)
        wavetables[7][i] = (static_cast<float>(rand()) / RAND_MAX) * 2.0f - 1.0f;
    }

    // Give every voice oscillator a pointer to our shared wavetable bank
    for (auto& voice : voices)
        voice.setWavetableBank(wavetables);
}

juce::AudioProcessorValueTreeState::ParameterLayout WaveForgeAudioProcessor::createParameterLayout()
{
    std::vector<std::unique_ptr<juce::RangedAudioParameter>> params;

    // Oscillator 1
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc1Pos", "Osc1 Wavetable",
        juce::NormalisableRange<float>(0.0f, 7.0f, 0.01f), 0.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc1Level", "Osc1 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 1.0f));

    // Oscillator 2
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2Pos", "Osc2 Wavetable",
        juce::NormalisableRange<float>(0.0f, 7.0f, 0.01f), 2.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2Level", "Osc2 Level",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "osc2Detune", "Osc2 Detune",
        juce::NormalisableRange<float>(-100.0f, 100.0f, 0.1f), 5.0f));

    // Oscillator Mix
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "oscMix", "Osc Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));

    // Filter
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterCutoff", "Filter Cutoff",
        juce::NormalisableRange<float>(20.0f, 20000.0f, 1.0f, 0.25f), 5000.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterRes", "Filter Resonance",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterEnvAmt", "Filter Env Amount",
        juce::NormalisableRange<float>(-1.0f, 1.0f, 0.01f), 0.5f));

    // Amp Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampAttack", "Amp Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampDecay", "Amp Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.1f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampSustain", "Amp Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.8f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "ampRelease", "Amp Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f), 0.3f));

    // Filter Envelope
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterAttack", "Filter Attack",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.01f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterDecay", "Filter Decay",
        juce::NormalisableRange<float>(0.001f, 5.0f, 0.001f, 0.3f), 0.3f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterSustain", "Filter Sustain",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "filterRelease", "Filter Release",
        juce::NormalisableRange<float>(0.001f, 10.0f, 0.001f, 0.3f), 0.5f));

    // Effects
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbMix", "Reverb Mix",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.2f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "reverbSize", "Reverb Size",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.5f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "chorusRate", "Chorus Rate",
        juce::NormalisableRange<float>(0.1f, 10.0f, 0.01f), 1.0f));
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "chorusDepth", "Chorus Depth",
        juce::NormalisableRange<float>(0.0f, 1.0f, 0.01f), 0.0f));

    // Master
    params.push_back(std::make_unique<juce::AudioParameterFloat>(
        "masterGain", "Master Gain",
        juce::NormalisableRange<float>(-24.0f, 12.0f, 0.1f), 0.0f));

    return { params.begin(), params.end() };
}

//==============================================================================
// Preset Management
//==============================================================================
juce::String WaveForgeAudioProcessor::getPresetName(int index) const
{
    if (index < NUM_BUILTIN_PRESETS)
        return builtInPresets[index].name;
    int userIdx = index - NUM_BUILTIN_PRESETS;
    if (userIdx < (int)userPresets.size())
        return userPresets[userIdx].name;
    return {};
}

void WaveForgeAudioProcessor::applyPreset(int index)
{
    currentProgram = index;

    if (index < NUM_BUILTIN_PRESETS)
    {
        const auto& preset = builtInPresets[index];
        for (int i = 0; i < (int)PARAM_IDS.size(); ++i)
        {
            auto* param = apvts.getParameter(PARAM_IDS[i]);
            if (param)
            {
                auto range = apvts.getParameterRange(PARAM_IDS[i]);
                param->setValueNotifyingHost(range.convertTo0to1(preset.values[i]));
            }
        }
        return;
    }

    int userIdx = index - NUM_BUILTIN_PRESETS;
    if (userIdx < (int)userPresets.size())
    {
        auto xml = juce::XmlDocument::parse(userPresets[userIdx].file);
        if (xml && xml->hasTagName(apvts.state.getType()))
            apvts.replaceState(juce::ValueTree::fromXml(*xml));
    }
}

void WaveForgeAudioProcessor::saveUserPreset(const juce::String& name)
{
    auto folder = getUserPresetsFolder();
    folder.createDirectory();

    auto state = apvts.copyState();
    state.setProperty("presetName", name, nullptr);
    auto xml = state.createXml();

    auto file = folder.getChildFile(name + ".wfpreset");
    xml->writeTo(file);

    loadUserPresetsFromDisk();
}

void WaveForgeAudioProcessor::loadUserPresetsFromDisk()
{
    userPresets.clear();
    auto folder = getUserPresetsFolder();
    if (folder.isDirectory())
    {
        for (auto& f : folder.findChildFiles(juce::File::findFiles, false, "*.wfpreset"))
        {
            UserPreset up;
            up.name = f.getFileNameWithoutExtension();
            up.file = f;
            userPresets.push_back(up);
        }
    }
}

juce::File WaveForgeAudioProcessor::getUserPresetsFolder() const
{
    return juce::File::getSpecialLocation(juce::File::userDocumentsDirectory)
               .getChildFile("WaveForge/Presets");
}

//==============================================================================
// AudioProcessor Interface
//==============================================================================
const juce::String WaveForgeAudioProcessor::getName() const { return JucePlugin_Name; }
bool WaveForgeAudioProcessor::acceptsMidi()  const { return true; }
bool WaveForgeAudioProcessor::producesMidi() const { return false; }
bool WaveForgeAudioProcessor::isMidiEffect() const { return false; }
double WaveForgeAudioProcessor::getTailLengthSeconds() const { return 2.0; }

int WaveForgeAudioProcessor::getNumPrograms()     { return getTotalNumPresets(); }
int WaveForgeAudioProcessor::getCurrentProgram()  { return currentProgram; }

void WaveForgeAudioProcessor::setCurrentProgram(int index)
{
    if (index >= 0 && index < getTotalNumPresets())
        applyPreset(index);
}

const juce::String WaveForgeAudioProcessor::getProgramName(int index)
{
    return getPresetName(index);
}

void WaveForgeAudioProcessor::changeProgramName(int index, const juce::String& newName)
{
    int userIdx = index - NUM_BUILTIN_PRESETS;
    if (userIdx >= 0 && userIdx < (int)userPresets.size())
    {
        auto newFile = userPresets[userIdx].file.getSiblingFile(newName + ".wfpreset");
        userPresets[userIdx].file.moveFileTo(newFile);
        userPresets[userIdx].name = newName;
        userPresets[userIdx].file = newFile;
    }
}

void WaveForgeAudioProcessor::prepareToPlay(double sampleRate, int samplesPerBlock)
{
    currentSampleRate = sampleRate;

    for (auto& voice : voices)
        voice.prepare(sampleRate, samplesPerBlock);

    // Re-share wavetable bank after prepare (voices reset their oscillators)
    for (auto& voice : voices)
        voice.setWavetableBank(wavetables);

    juce::dsp::ProcessSpec spec;
    spec.sampleRate = sampleRate;
    spec.maximumBlockSize = static_cast<juce::uint32>(samplesPerBlock);
    spec.numChannels = 2;

    reverb.prepare(spec);
    chorus.prepare(spec);
}

void WaveForgeAudioProcessor::releaseResources()
{
    for (auto& voice : voices)
        voice.reset();
}

#ifndef JucePlugin_PreferredChannelConfigurations
bool WaveForgeAudioProcessor::isBusesLayoutSupported(const BusesLayout& layouts) const
{
    if (layouts.getMainOutputChannelSet() != juce::AudioChannelSet::stereo())
        return false;
    return true;
}
#endif

SynthVoice* WaveForgeAudioProcessor::findFreeVoice()
{
    for (auto& voice : voices)
        if (!voice.isActive())
            return &voice;

    // Steal the voice that has been playing longest (voices[0] wraps around naturally)
    return &voices[0];
}

SynthVoice* WaveForgeAudioProcessor::findVoicePlayingNote(int note)
{
    for (auto& voice : voices)
        if (voice.isActive() && voice.getCurrentNote() == note)
            return &voice;
    return nullptr;
}

void WaveForgeAudioProcessor::handleMidiEvent(const juce::MidiMessage& msg)
{
    if (msg.isNoteOn())
    {
        auto* voice = findFreeVoice();
        if (voice)
            voice->noteOn(msg.getNoteNumber(), msg.getFloatVelocity());
    }
    else if (msg.isNoteOff())
    {
        auto* voice = findVoicePlayingNote(msg.getNoteNumber());
        if (voice)
            voice->noteOff();
    }
    else if (msg.isPitchWheel())
    {
        pitchBendValue = (msg.getPitchWheelValue() - 8192) / 8192.0f;
    }
    else if (msg.isAllNotesOff() || msg.isAllSoundOff())
    {
        // FL Studio stop button sends these — silence everything immediately
        for (auto& voice : voices)
            voice.reset();
        pitchBendValue = 0.0f;
    }
    else if (msg.isController() && msg.getControllerNumber() == 123)
    {
        // All Notes Off (belt-and-suspenders for hosts that send raw CC)
        for (auto& voice : voices)
            voice.reset();
    }
}

void WaveForgeAudioProcessor::processBlock(juce::AudioBuffer<float>& buffer, juce::MidiBuffer& midiMessages)
{
    juce::ScopedNoDenormals noDenormals;
    buffer.clear();

    // Handle MIDI
    for (const auto metadata : midiMessages)
        handleMidiEvent(metadata.getMessage());

    // Get parameters
    float osc1Pos    = apvts.getRawParameterValue("osc1Pos")->load();
    float osc1Level  = apvts.getRawParameterValue("osc1Level")->load();
    float osc2Pos    = apvts.getRawParameterValue("osc2Pos")->load();
    float osc2Level  = apvts.getRawParameterValue("osc2Level")->load();
    float osc2Detune = apvts.getRawParameterValue("osc2Detune")->load();
    float oscMix     = apvts.getRawParameterValue("oscMix")->load();

    float filterCutoff = apvts.getRawParameterValue("filterCutoff")->load();
    float filterRes    = apvts.getRawParameterValue("filterRes")->load();
    float filterEnvAmt = apvts.getRawParameterValue("filterEnvAmt")->load();

    float ampADSR[4] = {
        apvts.getRawParameterValue("ampAttack")->load(),
        apvts.getRawParameterValue("ampDecay")->load(),
        apvts.getRawParameterValue("ampSustain")->load(),
        apvts.getRawParameterValue("ampRelease")->load()
    };

    float filterADSR[4] = {
        apvts.getRawParameterValue("filterAttack")->load(),
        apvts.getRawParameterValue("filterDecay")->load(),
        apvts.getRawParameterValue("filterSustain")->load(),
        apvts.getRawParameterValue("filterRelease")->load()
    };

    float reverbMix  = apvts.getRawParameterValue("reverbMix")->load();
    float reverbSize = apvts.getRawParameterValue("reverbSize")->load();
    float chorusDepth = apvts.getRawParameterValue("chorusDepth")->load();
    float chorusRate  = apvts.getRawParameterValue("chorusRate")->load();
    float masterGain  = juce::Decibels::decibelsToGain(apvts.getRawParameterValue("masterGain")->load());

    // Render voices
    auto* leftChannel  = buffer.getWritePointer(0);
    auto* rightChannel = buffer.getWritePointer(1);
    int numSamples = buffer.getNumSamples();

    for (auto& voice : voices)
    {
        if (voice.isActive())
        {
            voice.renderNextBlock(leftChannel, rightChannel, numSamples,
                                  osc1Pos, osc2Pos, osc1Level, osc2Level,
                                  oscMix, osc2Detune, filterCutoff, filterRes,
                                  filterEnvAmt, ampADSR, filterADSR,
                                  0.5f, pitchBendValue);
        }
    }

    // Apply effects
    juce::dsp::AudioBlock<float> block(buffer);
    juce::dsp::ProcessContextReplacing<float> context(block);

    if (chorusDepth > 0.01f)
    {
        chorus.setRate(chorusRate);
        chorus.setDepth(chorusDepth);
        chorus.setMix(chorusDepth);
        chorus.process(context);
    }

    if (reverbMix > 0.01f)
    {
        juce::dsp::Reverb::Parameters reverbParams;
        reverbParams.roomSize  = reverbSize;
        reverbParams.wetLevel  = reverbMix;
        reverbParams.dryLevel  = 1.0f - reverbMix * 0.5f;
        reverbParams.damping   = 0.5f;
        reverb.setParameters(reverbParams);
        reverb.process(context);
    }

    buffer.applyGain(masterGain);
}

bool WaveForgeAudioProcessor::hasEditor() const { return true; }
juce::AudioProcessorEditor* WaveForgeAudioProcessor::createEditor() { return new WaveForgeAudioProcessorEditor(*this); }

void WaveForgeAudioProcessor::getStateInformation(juce::MemoryBlock& destData)
{
    auto state = apvts.copyState();
    state.setProperty("currentProgram", currentProgram, nullptr);
    std::unique_ptr<juce::XmlElement> xml(state.createXml());
    copyXmlToBinary(*xml, destData);
}

void WaveForgeAudioProcessor::setStateInformation(const void* data, int sizeInBytes)
{
    std::unique_ptr<juce::XmlElement> xmlState(getXmlFromBinary(data, sizeInBytes));
    if (xmlState && xmlState->hasTagName(apvts.state.getType()))
    {
        auto tree = juce::ValueTree::fromXml(*xmlState);
        currentProgram = tree.getProperty("currentProgram", 0);
        apvts.replaceState(tree);
    }
}

juce::AudioProcessor* JUCE_CALLTYPE createPluginFilter() { return new WaveForgeAudioProcessor(); }
