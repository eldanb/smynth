# smynth: Manifesto

This document attempts to provide a high level outline of this project, and the vision of the product we're trying to build.

# What we're building

smynth is an open-source hardware FM synthesizer. The preliminary features are going to be as follows.

## Tone-Generation
- [FM synthesis](https://en.wikipedia.org/wiki/Frequency_modulation_synthesis)-based tone generation. An FM voice may have the following characteristics:
  - Support for different algorithms with up to 6 operators
  - Each Operator supports multiple waveforms  
  - Operator frequency may be driven by note (+pitch bend), fixed, + offset by a controller with linear / logarithmic range, + offset by one of n LFOs, + offset by note velocity
  - Support for feedback modulation, with coefficient fixed / drive by a controller
  - A discrete attack and decay envelope for each operator, with multiple keyframe points to define the envelope.
  - Envelopes support time-loops
  - Envelopes may be time-scaled and amplitude-scaled by controllers and note velocity
  - Voice has a bandpass / lowpass / highpass filter. Possibly filter parameters may be controlled by a controller  
- Preferably 16 note polyphony, at least 2 timbres.

## Effects
- Parameterized Chorus and Reverb effects

## Physical Controls

- A physical on-board set of controllers allows live, intra-note change of voice parameters. Some may also function as input controls for programming
- 4 smooth rotary encoder knobs w/some sort of visual position display (assignable to voice parameters -- see above)
- Program Change/input encoder knob + 3 buttons for split mode: Primary voice toggle, Secondary voice toggle, Split mode + set split point button; indicator led next to each
- A 12 / 16 key "keyboard", supporting polyphony, with 5 additional buttons to change octaves -2..+2 (+ leds)
- 1 smooth analog joystick for pitch-bend + modulation
- 2x2 programmed effect buttons -- Chorus 1, Chorus 2, Reverb 1, Reverb 2 (+indicator leds)
- An LCD touch-screen for programing and interaction

## MIDI Support
- MIDI over USB support
- Support MIDI note commands; # of channels per number of timbres
- All assignable controllers (4 smooth controllers, pitch-bend, modulation) can be accepted over MIDI
- Program Change message over MIDI

## Interfaces 
- SD Card for sound libraries?

## Apregiators (?)
- Are they useful here?

## Audio Section
A high quality 16-bit, 44.1KHz DAC (minimum specs).

## Packaging
A "naked" PCB, no case :)

