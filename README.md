
[![License: GPL v3](https://img.shields.io/badge/License-GPLv3-blue.svg)](https://www.gnu.org/licenses/gpl-3.0)
[![JUCE](https://img.shields.io/badge/Built%20with-JUCE%208.0.12-blue)](https://juce.com)
[![Platform](https://img.shields.io/badge/Platform-Windows%20%7C%20-lightgrey)]()
[![Format](https://img.shields.io/badge/Format-VST3%20%7C%20-orange)]()
 ## Author: **William Ashley** — AlphaAudio  
##Project: *Balancer Adjust*
**Purpose:** Monitoring and adjusting audio, particularly Mid/Side processing.

# Balancer Adjust

A stereo Mid/Side utility plugin with per-channel EQ, width control, and monitoring tools.

Built with **JUCE 8** as a VST3/AU/AAX-compatible audio plugin. The DSP runs sample-by-sample in the M/S domain, giving you surgical control over the stereo field without leaving your DAW.

## What It Does

Balancer Adjust is a mastering and mix utility that lets you treat the **Mid** (mono) and **Side** (stereo difference) channels independently. It combines several stereo-processing tools into one compact interface:

| Section | Function |
|---------|----------|
| **Monitor / Gain** | Master gain fader, side gain (width) knob, and a monitor mode to solo Mid or Side for surgical listening. Includes a selectable summing law (-6 dB voltage or -3 dB power) for mono compatibility checks. |
| **Side Filters** | Dedicated HPF and LPF applied **only** to the Side channel — useful for removing muddy stereo low-end or taming excessive stereo highs. |
| **Bass Focus** | A quick elliptical filter that collapses low frequencies to mono below a user-defined crossover point. Handy for vinyl mastering or tightening up a mix's bottom end. |
| **M/S EQ Matrix** | Three parametric EQ bands, each routable to **Stereo**, **Mid only**, or **Side only**. Filter types include Bell, HPF, LPF, High Shelf, and Low Shelf. |

## Signal Flow
Stereo In → M/S Encode
├── Bass Focus (optional mono low-end)
├── Side HPF / LPF
├── M/S EQ Bands (routed per-band to M, S, or both)
├── Side Gain (width control)
├── Monitor Mode (Stereo / Solo Mid / Solo Side)
└── Master Gain → M/S Decode → Stereo Out 
 All filter coefficients are rebuilt on parameter change using JUCE's `dsp::IIR::Coefficients` and processed with independent filter state for Mid and Side channels.

## Factory Presets

Six built-in presets are baked into the binary as XML state strings:

| Preset | Purpose |
|--------|---------|
| **Default** | Flat starting point |
| **EDM / Synthwave Mud Cleanup** | Tightens low-mid buildup in wide synth pads |
| **Vocal Clarity / De-Boxing** | Targets boxy midrange in the Mid channel |
| **Reverb Tamer / De-Clouding** | Reduces washy stereo reverb tails |
| **Vinyl Mastering Prep** | Elliptical bass mono + gentle side roll-off |
| **Safe Width Booster** | Gentle side boost with protective HPF |

## UI Design

- **Dark charcoal / amber-gold** custom LookAndFeel (`AlphaLAF`) with subtle scanline texture
- Custom-drawn rotary knobs with arc indicators and LED-style toggle buttons
- Sectioned panel layout with color-coded accents (blue for Mid, teal for Side)
- Preset selector and bypass toggle in the top bar
- **620 × 520 px** editor window

## Technical Notes

- **Sample-accurate parameter smoothing** on master gain and side gain via `juce::LinearSmoothedValue`
- **Pure mono fallback** — if the host sends a single channel, the plugin bypasses M/S processing and applies gain only
- **APVTS-driven** — all parameters live in a `juce::AudioProcessorValueTreeState` with `SliderAttachment` / `ButtonAttachment` / `ComboBoxAttachment` bindings
- **State save/load** — full session recall via `ValueTree` serialization

## Building

Requires **JUCE 8** and a C++20-capable compiler. Standard JUCE plugin project structure — drop the `Source/` folder into a new JUCE audio plugin project and build.

## Quick Install (Windows)

The compiled VST3 is located a few folders down from the repo root. Copy the `.vst3` folder to: C:\Program Files\Common Files\VST3\


Then rescan plugins in your DAW. The plugin should appear as **Balancer Adjust**.




--------------------------------------------------------------------------------------------------
Copyright (c) 2026 William Ashley d/b/a William Ashley Music ( http://WilliamAshley.music )
This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License  (v3) 

This program is distributed in the hope that it will be useful to other audio programmers and music makers in their own plugin designs.
There is no WARRANTY expressed or implied including for MERCHANTABILITY or FITNESS FOR ANY PURPOSE. 
See the GNU General Public License for more details.

Attributtion is requested where possible if you use or modify any of the source,
Notice of use is requested so I can familiarize myself with how the code has been adapted for personal interest.
contact@WilliamAshley.music   
-----------------------------------------------------------------------------------------------------

## Third-Party Licenses

This project is built using the **JUCE framework**.

JUCE is © Raw Material Software Limited and is licensed under the terms of the JUCE End User License Agreement (EULA) or the JUCE Open Source License, as applicable.

- JUCE Website: https://juce.com
- JUCE Source Code: https://github.com/juce-framework/JUCE

This software may be distributed in VST3 format.

VST3 is a trademark of Steinberg Media Technologies GmbH. The VST3 SDK is licensed under the Steinberg VST 3 SDK License Agreement.

- Steinberg Website: https://www.steinberg.net
- VST3 SDK Source: https://github.com/steinbergmedia/vst3sdk

All trademarks and registered trademarks are the property of their respective owners.


