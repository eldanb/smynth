//
//  Waveforms.cpp
//  SmynthVoiceEngine
//
//  Created by Eldan Ben-Haim on 13/02/2024.
//

#include "Waveforms.hpp"
#include <math.h>

Waveforms::Waveforms()
{
    Waveforms::Sine = new SampleValue[WAVEFORM_LOOKUPTABLE_COUNT];
    for (int i = 0; i < WAVEFORM_LOOKUPTABLE_COUNT; i++)
    {
        Sine[i] = sin(((double)i / WAVEFORM_LOOKUPTABLE_COUNT) * (2 * 3.14)) * 32768;
    }
}

Waveforms Waveforms::waveformsInitializer;
SampleValue *Waveforms::Sine;
