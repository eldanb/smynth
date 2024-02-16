//
//  Waveforms.hpp
//  SmynthVoiceEngine
//
//  Created by Eldan Ben-Haim on 13/02/2024.
//

#pragma once

#include "FixedPoint.hpp"

#define WAVEFORM_LOOKUPTABLE_COUNT 4096

typedef SampleValue *WaveFormLookupTable;

class Waveforms
{
public:
    static SampleValue *Sine;

private:
    Waveforms();

    static Waveforms waveformsInitializer;
};

