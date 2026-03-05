# WaveForge - Wavetable Synthesizer VST3

A complete polyphonic wavetable synthesizer built with JUCE. 16 voices, dual oscillators with wavetable morphing, resonant filter with envelope, ADSR envelopes, and built-in effects.

## Architecture

```
MIDI → Voice Allocation (16 voices)
         ↓
    ┌─────────────────────────────────────┐
    │  OSC 1 (Wavetable) ──┐              │
    │                      ├→ MIX → FILTER → AMP ENV → Pan │
    │  OSC 2 (Wavetable) ──┘              │
    │         ↑                    ↑      │
    │      Detune            Filter Env   │
    └─────────────────────────────────────┘
         ↓
    Chorus → Reverb → Master Gain → Output
```

## Wavetables

8 built-in wavetables that you can morph between smoothly:

| Index | Name | Character |
|-------|------|-----------|
| 0 | **Sine** | Pure, clean fundamental |
| 1 | **Triangle** | Soft, hollow, flute-like |
| 2 | **Saw** | Bright, buzzy, rich harmonics |
| 3 | **Square** | Hollow, clarinet-like |
| 4 | **Pulse 25%** | Nasal, reedy |
| 5 | **Pulse 12.5%** | Thin, buzzy |
| 6 | **SuperSaw** | Thick, detuned, trance leads |
| 7 | **Noise** | Noise texture for percussion/FX |

Use the WAVE knob to morph continuously between adjacent wavetables.

## Controls

### Oscillator 1
- **WAVE** — Wavetable position (0-7, morphs between)
- **LEVEL** — Output level

### Oscillator 2
- **WAVE** — Wavetable position
- **LEVEL** — Output level
- **DETUNE** — Detune from OSC1 in cents (±100)

### Mix
- **MIX** — Crossfade between OSC1 and OSC2

### Filter
- **CUTOFF** — Filter cutoff frequency (20-20000 Hz)
- **RES** — Resonance (0-100%)
- **ENV** — Filter envelope amount (±100%)

### Amp Envelope
- **A** — Attack time
- **D** — Decay time
- **S** — Sustain level
- **R** — Release time

### Filter Envelope
- **A** — Attack time
- **D** — Decay time
- **S** — Sustain level
- **R** — Release time

### Effects
- **REV MIX** — Reverb wet/dry
- **REV SIZE** — Reverb room size
- **CHO RATE** — Chorus LFO rate
- **CHO DPTH** — Chorus depth

### Master
- **GAIN** — Output gain (±24 dB)

## Features

- **16-voice polyphony** with voice stealing
- **Pitch bend** support (±2 semitones)
- **Velocity sensitive**
- **Real-time wavetable visualization** in the UI
- **Live envelope visualization**
- **All parameters automatable**

## Preset Ideas

### Classic Pad
- OSC1: Sine (0), Level 1.0
- OSC2: Triangle (1), Level 0.5, Detune +7ct
- Filter: Cutoff 2000Hz, Res 0.2, Env 0.3
- Amp: A 0.5s, D 0.3s, S 0.7, R 1.0s
- Reverb: Mix 0.4, Size 0.7

### Trance Lead
- OSC1: SuperSaw (6), Level 1.0
- OSC2: Saw (2), Level 0.8, Detune +15ct
- Filter: Cutoff 8000Hz, Res 0.4, Env 0.5
- Amp: A 0.01s, D 0.2s, S 0.8, R 0.3s
- Filter Env: A 0.01s, D 0.4s, S 0.3, R 0.5s

### Bass
- OSC1: Square (3), Level 1.0
- OSC2: Saw (2), Level 0.6, Detune -5ct
- Filter: Cutoff 800Hz, Res 0.5, Env 0.7
- Amp: A 0.01s, D 0.1s, S 0.9, R 0.2s
- Filter Env: A 0.01s, D 0.3s, S 0.2, R 0.3s

### Pluck
- OSC1: Saw (2), Level 1.0
- OSC2: Pulse25 (4), Level 0.4, Detune +3ct
- Filter: Cutoff 3000Hz, Res 0.6, Env 0.8
- Amp: A 0.001s, D 0.3s, S 0.0, R 0.2s
- Filter Env: A 0.001s, D 0.2s, S 0.0, R 0.1s

### Noise Hit
- OSC1: Noise (7), Level 1.0
- OSC2: Square (3), Level 0.3
- Filter: Cutoff 5000Hz, Res 0.7, Env 0.9
- Amp: A 0.001s, D 0.1s, S 0.0, R 0.05s

## The UI

Synthwave-inspired cyan/purple color scheme with:
- Real-time wavetable waveform display
- Live ADSR envelope visualization
- Grid background aesthetic
- Monospace typography

## Building

```bash
cd WaveForge
cmake -B build -G "Visual Studio 17 2022"
cmake --build build --config Release
```

Copy `.vst3` from `build/WaveForge_artefacts/Release/VST3/` to:
- Windows: `C:\Program Files\Common Files\VST3\`
- macOS: `~/Library/Audio/Plug-Ins/VST3/`

## Usage in FL Studio

1. Install the VST3
2. In FL Studio: Add → Plugin database → Installed → Effects/Generators → WaveForge
3. Add to a MIDI track
4. Connect your MIDI keyboard or use FL's piano roll

## The Complete VoidVendor Suite

- **Lo-Fi Degrader** — Bit crush, sample crush, noise
- **Tape Wobble** — Wow, flutter, drift
- **Formant Filter** — Vocoder-style vowel filter
- **Frequency Destroyer** — Multiband annihilation
- **WaveForge** — Wavetable synthesizer

## License

MIT License

## Credits

Created by VoidVendor  
Built with [JUCE](https://juce.com/)  
Inspired by classic wavetable synths like PPG Wave, Waldorf, and Serum
