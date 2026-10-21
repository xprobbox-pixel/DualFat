# DUALFAT 1.2.0

This release embeds the supplied cyan/violet DUALFAT logo in the native plugin header. The audio engine, automation IDs and plugin identity are unchanged from 1.1.0.

## Installers

- **Mac:** `installer/package-macos.sh <Release-artifacts-directory> <destination>` creates a native `.pkg` for the architectures in the build. It installs AU/VST3 into the current user's `~/Library/Audio/Plug-Ins` and standalone into `~/Applications`. Quit your DAW before installation, then restart and rescan. Requires macOS 11 or later. Binaries are ad-hoc signed; the installer is not Developer ID signed or notarized.
- **Windows:** run `powershell -ExecutionPolicy Bypass -File scripts/build-windows.ps1` on Windows with Visual Studio 2022 C++ Desktop tools, CMake, Git and Inno Setup 6. It compiles x64 VST3 and standalone, runs DSP tests, and creates `dist/DUALFAT-1.2.0-Windows-x64-Setup.exe`. The installer uses the standard Common Files/VST3 directory and Program Files/DUALFAT, and provides an uninstaller. The C++ runtime is statically linked. Windows 10 or later; AU is Mac-only.
- **CI:** `.github/workflows/build.yml` builds both installers, validates Windows VST3 with pluginval and tests the Windows installer/uninstaller. Upload this project as the repository root and run **Build installers** under Actions. This workflow has been prepared but has not been run in this local Mac-only environment.

No Windows executable is included in the local source package until a Windows runner has compiled and validated it. The Mac package is a built native installer, not a source archive.

## Original build and plugin documentation

# DUALFAT 1.1.0

Native JUCE/C++ audio effect targeting VST3, macOS Audio Unit (AUv2), and Standalone.
The interface is drawn entirely with JUCE; every audio control is an APVTS parameter. Version 1.1.0 introduces the DUALFAT name, a darker neumorphic theme, sculpted metallic controls, dial-reactive LED rings, and embedded Roboto Regular/Bold. The DSP, host parameter IDs, VST3 class IDs, and AU identity are preserved from the working Cobalt Dual Filter version. The internal CMake target remains `CobaltDualFilter` for continuity.

## Build

Requirements: CMake 3.22+, Git, a C++17 compiler, and JUCE 8.0.12 (automatically fetched at a pinned commit). On macOS use current Xcode/Command Line Tools with macOS SDK **11.1 or newer**. The OS deployment target and SDK are different: installing an old SDK does not enable an Apple Silicon build.

From this project directory:

```sh
cmake -S . -B build -DCMAKE_BUILD_TYPE=Release
cmake --build build --config Release --parallel 4
ctest --test-dir build -C Release --output-on-failure
```

For an offline build, provide an existing JUCE checkout:

```sh
cmake -S . -B build -DJUCE_PATH=/absolute/path/to/JUCE -DCMAKE_BUILD_TYPE=Release
```

For a universal macOS build (Apple Silicon and Intel):

```sh
cmake -S . -B build-universal -DCMAKE_BUILD_TYPE=Release \
  -DCMAKE_OSX_ARCHITECTURES="arm64;x86_64" -DCMAKE_OSX_DEPLOYMENT_TARGET=11.0
cmake --build build-universal --config Release --parallel 4
```

On Windows use Visual Studio 2022 with Desktop development with C++, and the same basic commands. VST3 and Standalone are enabled there; AU is enabled only on macOS. Linux also requires JUCE's platform development packages (ALSA, X11, FreeType, and OpenGL); Linux builds have not been validated here.

Artifacts appear under `build/CobaltDualFilter_artefacts/Release/`:

- `VST3/DUALFAT.vst3`
- `AU/DUALFAT.component` (macOS)
- `Standalone/DUALFAT.app` (macOS), or the native executable on other systems

Copy the macOS VST3 bundle to `~/Library/Audio/Plug-Ins/VST3/` and the AU bundle to `~/Library/Audio/Plug-Ins/Components/`. Quit and reopen the DAW, then rescan plugins. Copy the entire bundle, not just its internal executable. The project deliberately does not install automatically.

The standalone application's JUCE audio settings select input/output devices and buffer size. JUCE initially mutes standalone input to prevent feedback; enable input after choosing suitable routing. Without a DAW, tempo sync uses **120 BPM**. Use Free Rate for independent standalone modulation.

## Controls and signal flow

`Input gain → HPF → LPF → dry/wet blend → output gain → global bypass blend`

The dry path includes input gain and output gain. Global bypass returns the original input at unity gain, with a short transition. Filter bypass/disable keeps the corresponding filter's internal state running, avoiding stale-state bursts when it is re-enabled. Enable and Bypass are separate automatable switches; a filter processes audibly only when Enabled is on and Bypass is off.

- **Cutoff:** exact logarithmic 20–20,000 Hz mapping. DSP frequency is capped at 45% of sample rate to remain below Nyquist. Drag the knob, type in its readout, or drag the graph's corresponding node horizontally. Double-click knobs to reset.
- **Resonance:** Q 0.5–8.0, applied to the first two-pole stage. Additional stages use Q ≈ 0.707 to avoid multiplying resonant peaks excessively.
- **Slope:** 12/24/36/48 dB per octave. All four cascaded stages stay warm; smoothed tap weights transition between slopes. These are cascades of resonant two-pole stages, not high-order Butterworth alignments.
- **LFO on:** enables cutoff modulation; off smoothly reduces modulation depth to zero.
- **Tempo sync:** host BPM determines the selected division. Available divisions are whole through thirty-second notes, each straight, dotted (×1.5 duration), or triplet (×2/3 duration). A whole-note cycle is four quarter-note beats, independently of the time signature.
- **Free rate:** 0.01–20 Hz, used when Tempo sync is off.
- **Depth:** 0–1, representing up to five octaves of cutoff movement. Bipolar moves above and below the base cutoff; Positive only upward; Negative only downward. The final cutoff remains bounded.
- **Phase:** 0–1 cycle offset. Sine, triangle, saw up/down, square, and deterministic sample-and-hold are available.
- **Retrigger:** with sync enabled and valid playing PPQ, locks phase to the DAW timeline, including loops/seeks. With free rate, resets on transport start or detected transport jumps. With Retrigger off, the oscillator runs continuously (while following BPM when sync is on). With the transport stopped or PPQ unavailable, it continues at the selected rate. A host without transport information cannot trigger resets.
- **Input/output:** −24 to +24 dB. Dry/wet uses a linear crossfade, suitable for correlated filter paths.

Both LFOs have separate phase and smoothing state. Sample-and-hold is hashed from the cycle number, so seeking to the same musical position reproduces the same value. Matching settings intentionally produce matching modulation; use phase/rate/division to offset the filters.

The graph shows each filter and their series product; it does not include master gain or the dry/wet blend. Its vertical scale is −60 to +24 dB. At 30 fps it reads the actual modulated cutoff from the audio thread. Q, enable and slope are displayed at their current parameter targets; brief 20 ms transitions are not plotted as intermediate transfer curves. Waveform displays show oscillator shape and moving phase, with dimming when modulation is off; the DSP smooths sharp waveform edges.

## Automation, state and presets

All 34 parameters are hosted through `AudioProcessorValueTreeState`, with stable IDs and version hints. Sliders, menus, switches, graph drags, and preset loads notify the host. The global bypass is also exposed as the host bypass parameter.

DAW sessions persist all parameters through JUCE XML/binary state. **SAVE** and **LOAD** use readable `.dualfat` XML files; existing `.cobalt` presets can still be loaded. Example presets are in `Presets/`. Preset file access happens on the message thread. Preset loads update parameters with host gestures and existing DSP smoothing. LFO run phase/filter history are transient; timeline-locked phase is reconstructed from host PPQ after loading.

## Implementation

- `Source/Dsp.h`: JUCE-independent stereo DSP, topology-preserving state-variable filters, LFOs, smoothing, transfer function.
- `Source/PluginProcessor.*`: APVTS layout, cached atomic parameter pointers, host tempo/transport handling, processing, state and preset persistence.
- `Source/PluginEditor.*`: custom neumorphic controls, resizable symmetrical panels, animated LFOs, interactive graph, attachments and asynchronous file chooser.
- `Tests/DspTests.cpp`: standalone DSP verification.
- `.github/workflows/build.yml`: macOS and Windows native build/test jobs; run after uploading this project to GitHub.

Audio processing uses fixed-size stack/member storage, no heap allocation or locks in project callback code, and independent filter state per channel. Host parameters are sampled once per callback, then smoothed per sample; JUCE's usual block-level automation delivery applies. LFOs and cutoff coefficients are calculated each sample. Normal parameter/bypass transitions use a 20 ms exponential time constant, with 3 ms smoothing of LFO waveform transitions. This trades instantaneous changes for smooth modulation. Processing adds no lookahead or reported latency. Mono and stereo matching input/output bus layouts are supported.

## Verification status

Updated and built locally on macOS 15.7.9 using Apple Command Line Tools 16.4, Apple Clang 17, macOS SDK 15.5, CMake 4.4.4, and pinned JUCE 8.0.12. Native **arm64** VST3, AUv2, and Standalone bundles are provided separately in `DUALFAT-macOS-arm64`. The deployment target is macOS 11.0. Intel/Rosetta builds are not included in that package.

**Passed:** native compilation/linking of all three formats; ad-hoc code-signature verification; CMake/CTest DSP suite; pluginval 1.0.4 strictness level 5 for VST3 (including editor, state, automation and processing tests); Apple's AU validator. The standalone launches and its interface has been visually checked. Standalone input remains muted by JUCE by default; select an audio device and routing before unmuting.

DSP tests cover stability at 22.05, 44.1, 48, 96 and 192 kHz during aggressive modulation/resonance/slope/bypass changes; analytical stopbands; measured audio versus graph transfer function for both filters and all slopes; stereo isolation; dry mix/global bypass convergence; PPQ phase synchronization. Validation logs are included with the binary package. This is a locally tested build, not a broad DAW compatibility certification.

The 16.4 updater left legacy libc++ headers from the previous 2020 toolchain installed on this machine. The build explicitly selects the matching SDK's libc++ headers; no JUCE SDK checks are bypassed. `scripts/build-macos.sh` applies this configuration for reproducibility. Set `CMAKE_BIN` to a CMake executable if it is not on PATH; optionally set `JUCE_PATH` to a local checkout and `COBALT_ARCHS="arm64;x86_64"` for a universal build. Final CMake bundle steps apply ad-hoc signatures after VST3 manifest generation.

To run only DSP tests without JUCE or network access:

```sh
cmake -S . -B build-dsp -DCOBALT_DSP_ONLY=ON
cmake --build build-dsp --config Release
ctest --test-dir build-dsp -C Release --output-on-failure
```

After building with a supported SDK, validate the installed AU with:

```sh
auval -v aufx Cdfl Cblt
```

For further release testing, use Steinberg's validator or higher pluginval strictness, test session recall in each intended DAW, and audition all slopes at extreme Q. High resonance and +24 dB gain can exceed full scale; the effect intentionally has no hidden limiter.

## Design and dependencies

The near-black surfaces, cyan HPF/violet LPF LEDs, recessed displays, and metallic rotary controls are original JUCE drawing code inspired by the supplied screenshot and [Dark Neumorphic UI Kit reference](https://ui8.net/creativetagx/products/dark-neumorphic-ui-kit). No paid UI-kit assets are bundled.

JUCE is downloaded separately at build time. Its own license applies; see the pinned checkout's `LICENSE.md` and ensure your intended distribution complies. Review signing/notarization and the JUCE licensing requirements before distributing commercial binaries.

Roboto is embedded from the official googlefonts/roboto-2 source; its license is included in `Assets/Fonts/LICENSE-Roboto.txt`. Regular and Bold faces are used throughout the plugin canvas, menus, labels, readouts, and tooltips. Native macOS window chrome and native file dialogs use system styling. Dial bodies/shadows are cached at 2x resolution; UI animation runs at 30 fps and follows parameter values. A white cutoff LED shows the modulated position when the LFO is active.
