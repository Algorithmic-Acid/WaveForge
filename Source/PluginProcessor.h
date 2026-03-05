#pragma once

#include <juce_audio_processors/juce_audio_processors.h>
#include <juce_dsp/juce_dsp.h>
#include <array>
#include <vector>

//==============================================================================
// Wavetable definitions
static constexpr int WAVETABLE_SIZE = 2048;
static constexpr int NUM_WAVETABLES = 8;
static constexpr int MAX_VOICES = 16;
static constexpr int NUM_BUILTIN_PRESETS = 8;

// Parameter IDs in a fixed order (used by preset system)
static const juce::StringArray PARAM_IDS = {
    "osc1Pos", "osc1Level", "osc2Pos", "osc2Level", "osc2Detune",
    "oscMix", "filterCutoff", "filterRes", "filterEnvAmt",
    "ampAttack", "ampDecay", "ampSustain", "ampRelease",
    "filterAttack", "filterDecay", "filterSustain", "filterRelease",
    "reverbMix", "reverbSize", "chorusRate", "chorusDepth", "masterGain"
};

//==============================================================================
// Preset
struct Preset
{
    juce::String name;
    std::array<float, 22> values; // matches PARAM_IDS order
};

//==============================================================================
// ADSR Envelope
class ADSREnvelope
{
public:
    enum class Stage { Idle, Attack, Decay, Sustain, Release };

    void setParameters(float attack, float decay, float sustain, float release, float sampleRate);
    void noteOn();
    void noteOff();
    float getNextValue();
    bool isActive() const { return stage != Stage::Idle; }
    void reset() { stage = Stage::Idle; currentValue = 0.0f; }

private:
    Stage stage = Stage::Idle;
    float currentValue = 0.0f;
    float attackRate = 0.0f;
    float decayRate = 0.0f;
    float sustainLevel = 1.0f;
    float releaseRate = 0.0f;
};

//==============================================================================
// Single Wavetable Oscillator
// Uses a pointer to the processor's shared wavetable bank (no redundant copy)
class WavetableOscillator
{
public:
    void prepare(double sampleRate);
    void setFrequency(float freq);
    void setWavetableBank(const std::array<std::array<float, WAVETABLE_SIZE>, NUM_WAVETABLES>& bank);
    void setWavetablePosition(float position); // 0-7 selects/morphs between tables
    float getNextSample();
    void reset() { phase = 0.0f; }

private:
    double sampleRate = 44100.0;
    float phase = 0.0f;
    float phaseIncrement = 0.0f;
    const std::array<std::array<float, WAVETABLE_SIZE>, NUM_WAVETABLES>* wavetableBank = nullptr;
    float tablePosition = 0.0f;
};

//==============================================================================
// Synth Voice
class SynthVoice
{
public:
    void prepare(double sampleRate, int samplesPerBlock);
    void setWavetableBank(const std::array<std::array<float, WAVETABLE_SIZE>, NUM_WAVETABLES>& bank);
    void noteOn(int midiNote, float velocity);
    void noteOff();
    bool isActive() const { return ampEnv.isActive(); }
    void renderNextBlock(float* outputL, float* outputR, int numSamples,
                         float osc1Pos, float osc2Pos, float osc1Level, float osc2Level,
                         float oscMix, float detune, float filterCutoff, float filterRes,
                         float filterEnvAmt, float* ampADSR, float* filterADSR,
                         float pan, float pitchBend);
    int getCurrentNote() const { return currentNote; }
    void reset();

private:
    double sampleRate = 44100.0;
    int currentNote = -1;
    float velocity = 0.0f;
    float baseFrequency = 440.0f;

    WavetableOscillator osc1, osc2;
    ADSREnvelope ampEnv, filterEnv;

    // Filter state (2-pole SVF)
    float filterState[4] = {0, 0, 0, 0};

    void processFilter(float& sample, float cutoff, float resonance);
};

//==============================================================================
// Main Processor
class WaveForgeAudioProcessor : public juce::AudioProcessor
{
public:
    WaveForgeAudioProcessor();
    ~WaveForgeAudioProcessor() override;

    void prepareToPlay(double sampleRate, int samplesPerBlock) override;
    void releaseResources() override;

#ifndef JucePlugin_PreferredChannelConfigurations
    bool isBusesLayoutSupported(const BusesLayout& layouts) const override;
#endif

    void processBlock(juce::AudioBuffer<float>&, juce::MidiBuffer&) override;

    juce::AudioProcessorEditor* createEditor() override;
    bool hasEditor() const override;

    const juce::String getName() const override;

    bool acceptsMidi() const override;
    bool producesMidi() const override;
    bool isMidiEffect() const override;
    double getTailLengthSeconds() const override;

    int getNumPrograms() override;
    int getCurrentProgram() override;
    void setCurrentProgram(int index) override;
    const juce::String getProgramName(int index) override;
    void changeProgramName(int index, const juce::String& newName) override;

    void getStateInformation(juce::MemoryBlock& destData) override;
    void setStateInformation(const void* data, int sizeInBytes) override;

    juce::AudioProcessorValueTreeState apvts;

    // Wavetable access for UI visualization
    const float* getWavetable(int index) const { return wavetables[index].data(); }
    static constexpr int getWavetableSize() { return WAVETABLE_SIZE; }

    // ------------------------------------------------------------------ Presets
    static const std::array<Preset, NUM_BUILTIN_PRESETS> builtInPresets;

    struct UserPreset
    {
        juce::String name;
        juce::File   file;
    };

    void applyPreset(int index);
    void saveUserPreset(const juce::String& name);
    void loadUserPresetsFromDisk();
    juce::File getUserPresetsFolder() const;

    int getTotalNumPresets() const { return NUM_BUILTIN_PRESETS + (int)userPresets.size(); }
    juce::String getPresetName(int index) const;
    const std::vector<UserPreset>& getUserPresets() const { return userPresets; }

private:
    juce::AudioProcessorValueTreeState::ParameterLayout createParameterLayout();
    void initializeWavetables();
    void handleMidiEvent(const juce::MidiMessage& msg);
    SynthVoice* findFreeVoice();
    SynthVoice* findVoicePlayingNote(int note);

    // Wavetables shared with all oscillators: Sine, Triangle, Saw, Square, Pulse25, Pulse12, SuperSaw, Noise
    std::array<std::array<float, WAVETABLE_SIZE>, NUM_WAVETABLES> wavetables;

    // Polyphonic voices
    std::array<SynthVoice, MAX_VOICES> voices;

    // Global state
    double currentSampleRate = 44100.0;
    float pitchBendValue = 0.0f; // -1 to +1

    // Preset state
    int currentProgram = 0;
    std::vector<UserPreset> userPresets;

    // Effects
    juce::dsp::Reverb reverb;
    juce::dsp::Chorus<float> chorus;

    JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(WaveForgeAudioProcessor)
};
