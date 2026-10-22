==========================================================================
                              D U A L F A T
                       Dual Resonant Filter Effect
                         USER GUIDE | VERSION 1.2.1
==========================================================================

DUALFAT shapes sound with two independent filters: a cyan High-Pass Filter
(HPF) and a violet Low-Pass Filter (LPF). Use either filter on its own, or
combine them to isolate a moving band of frequencies. Each filter has its
own LFO for gentle movement, rhythmic sweeps and more dramatic modulation.

Designed for mono and stereo audio, DUALFAT includes host automation,
saveable presets, a live filter-response display and a dark interface with
illuminated controls. The audio engine uses smoothed parameter changes and
reports zero processing latency.


1. INSTALLATION
--------------------------------------------------------------------------

MAC - macOS 11 or later; Apple Silicon and Intel

  1) Close your DAW.
  2) Open the supplied .pkg installer and follow its instructions.
  3) Restart your DAW and rescan plug-ins if needed.

  Installed for the current user:

    AU:          ~/Library/Audio/Plug-Ins/Components/DUALFAT.component
    VST3:        ~/Library/Audio/Plug-Ins/VST3/DUALFAT.vst3
    Standalone:  ~/Applications/DUALFAT.app
    Presets:     ~/Library/Application Support/DUALFAT/Presets

  The ~ symbol means your home folder.

WINDOWS - 64-bit Windows 10 or later

  1) Close your DAW.
  2) Run the supplied Windows x64 Setup.exe installer.
  3) Restart your DAW and rescan VST3 plug-ins if needed.

  VST3 installs into:
    C:\Program Files\Common Files\VST3\DUALFAT.vst3

  The standalone application and presets install into:
    C:\Program Files\DUALFAT

  A Start-menu shortcut and uninstaller are included. AU is Mac-only.

RELEASE NOTE

  These installers are unsigned. The Mac application and plug-ins are
  ad-hoc signed, but the package is not Apple-notarized. Your operating
  system may request approval before installation or launch.


2. QUICK START IN ABLETON LIVE
--------------------------------------------------------------------------

  1) Find DUALFAT in Live's Plug-Ins browser. On Mac, choose either AU or
     VST3; on Windows, choose VST3.
  2) Drag it onto an audio track, a return track, or after an instrument.
     DUALFAT is an audio effect; it needs incoming audio to make sound.
  3) Start playback. Keep the global BYPASS switch off.
  4) Set DRY / WET to 100% to hear the filtered signal clearly.
  5) Turn HPF CUTOFF upward to remove more low frequencies.
  6) Turn LPF CUTOFF downward to remove more high frequencies.
  7) Adjust OUTPUT to match the perceived level of the unprocessed sound.

  If DUALFAT is missing, check that the relevant AU or VST3 scanning option
  is enabled in Live's plug-in settings, then rescan and restart Live.
  After an update, close and reopen Live so it loads the new version.


3. THE FILTER PANELS
--------------------------------------------------------------------------

HIGH PASS - CYAN
  Passes frequencies above its cutoff and reduces frequencies below it.
  Useful for reducing rumble, thinning a sound or building rising sweeps.

LOW PASS - VIOLET
  Passes frequencies below its cutoff and reduces frequencies above it.
  Useful for softening brightness, creating darker tones or closing sweeps.

CUTOFF / Hz
  Sets the filter's base frequency from 20 Hz to 20 kHz on a logarithmic
  scale. With an LFO active, this is the centre or starting reference for
  the movement, depending on the selected polarity.

RESONANCE / Q
  Emphasises frequencies around the cutoff. The range is Q 0.5 to 8.0.
  Around 0.71 is a useful starting point. Higher values produce a more
  pronounced peak and can raise the signal level considerably.

SLOPE
  Select 12, 24, 36 or 48 dB/oct. Higher slopes remove frequencies more
  steeply beyond the cutoff. Try 12 dB/oct for gentle shaping and 24 or
  48 dB/oct for stronger separation.

ENABLE AND BYPASS
  A filter processes audio when ENABLE is on and its BYPASS is off.
  Turning ENABLE off or BYPASS on removes that filter from the audible
  path while preserving its settings. The other filter remains independent.

USING BOTH FILTERS
  The HPF feeds the LPF. With the HPF cutoff below the LPF cutoff, the
  frequencies between them form a variable pass band. Bring the cutoffs
  closer together for a narrower band. Crossing them can greatly reduce
  the level; it does not automatically swap their positions.

WINDOW SIZE
  Use the UI SIZE menu at the top to choose 50%, 60%, 75%, 85%, 100%,
  125% or 150%. You can also drag the bottom-right corner to shrink or
  enlarge the whole interface. Every control scales together. The default
  is 75% (840 x 741); the minimum is 50% (560 x 494). The last size is
  remembered while this plugin instance remains loaded.

DIAL OPERATION
  Drag a dial or enter a value in its readout. Double-click a rotary dial
  to restore its default value. Its illuminated ring follows the setting;
  the cutoff modulation indicator also follows active LFO movement.


4. INDEPENDENT LFOs
--------------------------------------------------------------------------

An LFO is a slow control signal that moves a filter's cutoff automatically.
Each panel has its own LFO; changing one does not change the other.

LFO
  Enables cutoff modulation. Increase DEPTH above zero to hear movement.

SYNC
  On: use the host tempo and selected musical division.
  Off: use the FREE RATE / Hz dial, from 0.01 to 20 cycles per second.
  The free-rate dial does not set the speed while SYNC is on.

DIVISION AND TIMING
  Choose 1/1, 1/2, 1/4, 1/8, 1/16 or 1/32, with Straight, Dotted or Triplet
  timing. These are note-length cycles. For example, at 120 BPM a straight
  1/4 cycle lasts 0.5 seconds. Dotted cycles last 1.5 times as long;
  triplet cycles last two-thirds as long.

WAVEFORM
  Sine       Smooth, rounded movement.
  Triangle   Even rises and falls.
  Saw up     Repeated upward sweeps.
  Saw down   Repeated downward sweeps.
  Square     Alternates between two modulation levels.
  S&H        Sample-and-hold: a new stepped value each cycle.

DEPTH / 5 OCT
  Sets the modulation amount on a 0.00 to 1.00 scale. At 1.00, the maximum
  excursion is five octaves in each permitted direction. In Bipolar mode
  that means up to five octaves above and below the base cutoff. Start
  around 0.10 to 0.25 for easier control. Actual movement stops at the
  filter's frequency limits.

PHASE / CYCLE
  Offsets the waveform by 0.00 to 1.00 cycle. A value of 0.50 shifts it by
  half a cycle. Different phase settings can make the two filters move
  together or in opposing directions.

POLARITY
  Bipolar    Moves above and below the base cutoff.
  Positive   Moves from the base cutoff upward.
  Negative   Moves from the base cutoff downward.

RETRIG
  With SYNC and RETRIG on during playback, the LFO follows the host's
  musical timeline. In free-rate mode, RETRIG restarts the cycle when
  transport starts or a sufficiently large position jump is detected.
  With RETRIG off, the LFO runs continuously while audio is processed;
  SYNC still makes its speed follow the host tempo.

  This is transport retriggering, not MIDI-note retriggering. Behaviour
  while stopped depends on whether the host continues processing audio.
  If the host supplies no tempo, DUALFAT uses a 120 BPM fallback.

MODULATOR DISPLAY
  Shows the selected LFO shape and its movement. It is a modulation
  display, not an audio waveform or oscilloscope.


5. MASTER CONTROLS AND RESPONSE GRAPH
--------------------------------------------------------------------------

INPUT / OUTPUT
  Each provides -24 to +24 dB of gain. INPUT changes the level entering
  the effect; OUTPUT adjusts the level leaving it. These are gain controls,
  not dedicated saturation or distortion controls.

DRY / WET
  0% gives the unfiltered signal; 100% gives the filtered signal. Values
  between them blend both paths. Input and output gain also affect the
  dry path, so 0% wet is not necessarily identical to global bypass.

GLOBAL BYPASS
  Bypasses the entire effect, including its input/output gains and mix,
  returning to the original incoming signal with a smoothed transition.

FILTER RESPONSE
  The top graph shows the individual HPF and LPF responses and their
  combined filter response. It updates with cutoff, resonance, slope and
  LFO movement. Drag the coloured cutoff nodes to tune the filters.

  This is a calculated filter-response graph, not a spectrum analyser or
  output-level meter. It does not show the final dry/wet and gain response.


6. PRESETS, AUTOMATION AND SESSION RECALL
--------------------------------------------------------------------------

SAVE / LOAD
  SAVE writes the current settings to a .dualfat preset file. LOAD opens a
  saved preset. Earlier .cobalt presets are also accepted. Use LOAD to
  browse the factory preset folder listed under Installation.

  Included starting points:
    01 Clean Band
    02 Slow Motion
    03 Rhythmic Low Pass

DAW AUTOMATION
  The filter, LFO and master parameters are available for host automation.
  In Live, expose the desired plug-in parameters using the device's
  Configure function, then record or draw automation as usual.

SESSION RECALL
  Saving your DAW project stores DUALFAT's parameter settings with it.
  You do not need to save a separate preset for normal project recall.
  Version 1.2 retains the plugin identity and parameter IDs used by the
  earlier DUALFAT releases.


7. STARTING POINTS TO TRY
--------------------------------------------------------------------------

GENTLE CLEANUP
  Disable the LPF. Start with HPF cutoff around 30-60 Hz, Q around 0.71
  and a 12 or 24 dB/oct slope. Adjust by ear for the source material.

DARKER TONE
  Disable the HPF. Set the LPF between 3 and 8 kHz with moderate resonance.
  Lower the cutoff until the sound sits where you want it.

RHYTHMIC LOW-PASS
  Disable the HPF. Set LPF cutoff near 1 kHz, Q near 1, and slope to 24.
  Turn on LFO, SYNC and RETRIG. Choose Sine, 1/8, Straight and Bipolar.
  Start with DEPTH at 0.20, then adjust the cutoff and depth together.

MOVING BAND
  Enable both filters. Start with HPF around 200 Hz and LPF around 4 kHz.
  Use slow sine LFOs at modest depth. Try matching rates, then offset one
  phase by 0.50 for opposing movement. Keep an ear on the output level
  when the cutoffs approach or cross each other.

PARALLEL COLOUR
  Make a strong filter setting, then reduce DRY / WET to blend it with
  the original signal. Compare with global BYPASS and level-match OUTPUT.


8. STANDALONE USE AND TROUBLESHOOTING
--------------------------------------------------------------------------

STANDALONE AUDIO
  Open DUALFAT from your Applications folder or Windows Start menu.
  Use Options / audio settings to select your input, output and buffer
  size. The standalone version processes incoming audio; it has no built-in
  sound source or audio-file player.

  Audio input may initially be muted to avoid a feedback loop. Configure
  routing before unmuting; use headphones when monitoring a microphone.
  Without a DAW tempo source, SYNC uses 120 BPM. Turn SYNC off to choose
  the speed directly in Hz.

NO SOUND OR VERY LOW LEVEL
  Check incoming audio and device routing. Confirm global BYPASS is off
  when testing the effect. Check INPUT, OUTPUT and the two cutoff values.
  Crossed or very narrow filter settings can remove most of the signal.

NO LFO MOVEMENT IN THE SOUND
  Turn LFO on, increase DEPTH, and ensure the relevant filter is enabled
  and not bypassed. Increase DRY / WET. Start host playback for timeline-
  locked modulation. Turn SYNC off if you want FREE RATE to set the speed.

LOUD PEAKS OR CLIPPING
  Reduce resonance, INPUT or OUTPUT. DUALFAT has no dedicated limiter;
  watch your DAW's meters when using high Q or extreme sweeps.

CUTOFF DOES NOT REACH 20 kHz
  At lower sample rates, the DSP caps cutoff at 45% of the sample rate
  for stability. For example, at 44.1 kHz the upper limit is 19.845 kHz.
  The base parameter still offers the full 20 Hz-20 kHz range.

UNINSTALL
  Mac: remove DUALFAT.component and DUALFAT.vst3 from the user plug-in
  folders listed above, and remove the standalone app if no longer needed.
  Windows: use Installed Apps to uninstall DUALFAT.
  Keep any personal presets you want to reuse.


PROJECT AND SOURCE
--------------------------------------------------------------------------
  https://github.com/xprobbox-pixel/DualFat

==========================================================================
                    DUALFAT 1.2.1 | END OF USER GUIDE
==========================================================================
