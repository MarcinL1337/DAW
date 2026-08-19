# DAW — Desktop Audio Workstation

A simple and intuitive desktop DAW (Digital Audio Workstation) built in C++ using the JUCE framework.
Developed as an engineering thesis project at the University of Wrocław, Faculty of Mathematics and Computer Science.

> **Authors:** Marcin Linkiewicz, Julian Włodarek
> **Supervisor:** dr hab. Paweł Woźny

---

## About

DAW is a lightweight alternative to complex, feature-overloaded audio workstations.
It targets beginner audio creators who need essential tools without a steep learning curve.
The app lets you go from idea to a first draft quickly, without spending hours learning the software.

---

## Features

- Import audio files in various formats with automatic resampling
- Multi-track audio management with waveform visualization
- Audio effects per track: **Gain**, **Panner**, **Reverb** (with full parameter control)
- **Fade in / Fade out** with 4 curve types: linear, logarithmic, exponential, S-curve
- **Split** clips into smaller parts
- **Drag & drop** clips within and between tracks
- Playhead with three follow modes: No Follow, Smooth Follow, Jump Follow
- Track controls: **Mute** and **Solo**
- Project save/load with JSON serialization and automatic audio file copying
- Keyboard shortcuts for all major actions

---

## Tech Stack

| Technology | Purpose |
|---|---|
| C++23 | Core language |
| JUCE | Audio engine & GUI framework |
| CMake | Build system |
| ClangFormat | Code formatting |
| nlohmann/json | Project serialization |

**Architecture:** Observer + Mediator design patterns, JUCE `ValueTree` for state management,
`AudioProcessorGraph` for flexible audio routing.

---

## Building

The project was developed and tested on **Windows 11**.
Linux builds (e.g. Ubuntu 22.04) are possible but not officially guaranteed.

### Windows (CLion)

1. Install [CLion](https://www.jetbrains.com/clion/), MSVC compiler, and Windows SDK
2. Open the project folder in CLion
3. Build using the built-in CMake integration (`CMakeLists.txt`)

### Linux (Ubuntu 22.04)

```bash
# Clone the repository
git clone https://github.com/MarcinL1337/DAW.git
cd DAW

# Install dependencies
sudo apt update
sudo apt install -y build-essential cmake ninja-build pkg-config \
  libfreetype6-dev libfontconfig1-dev libx11-dev libxrandr-dev \
  libxinerama-dev libxcursor-dev libasound2-dev libjack-jackd2-dev \
  libcurl4-openssl-dev libglu1-mesa-dev freeglut3-dev mesa-common-dev \
  libwebkit2gtk-4.1-dev libgtk-3-dev

# Install GCC 13+ (required for C++23)
sudo add-apt-repository ppa:ubuntu-toolchain-r/test
sudo apt update
sudo apt install g++-13

# Build
cmake -S . -B build -G Ninja -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_CXX_COMPILER=g++-13 -DCMAKE_C_COMPILER=gcc-13
cmake --build build -j
```

---

## Keyboard Shortcuts

| Shortcut | Action |
|---|---|
| `Space` | Play / Pause |
| `Backspace` | Stop & reset playback |
| `Ctrl+N` | New project |
| `Ctrl+O` | Open project |
| `Ctrl+S` | Save project |
| `Ctrl+Shift+S` | Save project as |
| `Ctrl+Shift+A` | Add audio file |
| `Shift` | Add new empty track |
| `Ctrl+H` | Open user guide |

## License

This project is licensed under the **GNU General Public License v3.0** (GPL-3.0).

This means you are free to use, study, modify, and distribute this software,
provided that any derivative works are also released under the GPL-3.0 license.
