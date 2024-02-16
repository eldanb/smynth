//
//  SmynthVoiceEngine.cpp
//  SmynthVoiceEngine
//
//  Created by Eldan Ben-Haim on 29/07/2023.
//

#include "SmynthVoiceEngine.h"
#include "smynth-voice-engine/include/SyEngine.hpp"

SyEngine *engine = nullptr;

void fillSampleBuffer(int16_t *buffer, int numChannels, int numFrames) {
    if(!engine) {
        engine = new SyEngine();
    }
    engine->renderSamples(buffer, numChannels, numFrames);
}

