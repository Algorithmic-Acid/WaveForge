# Contributing to WaveForge

Thanks for your interest in contributing! Here's what you need to know.

## How to Contribute

1. **Fork** the repository
2. **Create a branch** from `main` — name it something descriptive (`fix/filter-click`, `feature/lfo`)
3. **Make your changes** and test them in a DAW (FL Studio, Reaper, etc.)
4. **Open a Pull Request** against `main`
5. Wait for review — all PRs require approval before merging

## Pull Request Rules

- All PRs must be reviewed and approved by a maintainer before merging
- Direct pushes to `main` are disabled — no exceptions
- One clear purpose per PR (don't bundle unrelated changes)
- If adding a feature, describe why it fits WaveForge's direction

## Build Requirements

- CMake 3.22+
- Visual Studio 2022 (Windows) / Xcode (macOS) / GCC/Clang (Linux)
- Git (for JUCE FetchContent)
- Internet connection on first build (downloads JUCE 8)

```bash
cmake -B build
cmake --build build --config Release --target WaveForge_VST3
```

The built VST3 will be at:
`build/WaveForge_artefacts/Release/VST3/WaveForge.vst3`

## Code Style

- C++17
- Follow the existing code structure (processor/editor split)
- Keep DSP code in `PluginProcessor.cpp`, UI code in `PluginEditor.cpp`
- No external dependencies beyond JUCE

## Reporting Bugs

Open a GitHub Issue with:
- DAW and version
- OS and version
- Steps to reproduce
- Expected vs actual behaviour
