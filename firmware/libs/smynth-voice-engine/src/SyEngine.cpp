#include "SyEngine.hpp"
#include <math.h>

#include "FixedPoint.hpp"
#include "Waveforms.hpp"
#include "ToneGenerator.hpp"

#define SAMPLING_RATE 44100


SyEngine::SyEngine()
{
    configureToneGenerator();
}

void SyEngine::configureToneGenerator()
{
    ToneGeneratorConfig config = {
        .nodeConfig = {
            {.oscillatorConfig = {
                .baseArgDelta = ((double)440 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT,
                .waveform = Waveforms::Sine,
                .volumeEnvelope = {
                    .numKeyFrames = 3,
                    .keyFrames = {
                        {
                            .sampleTime = 0,
                            .envelopeValue = 0.05
                        },
                        {
                            .sampleTime = 22050,
                            .envelopeValue = 0.02
                        },
                        {
                            .sampleTime = 44100,
                            .envelopeValue = 0.05
                        }
                    }
                }
                }},
            {.oscillatorConfig = {.baseArgDelta = ((double)440 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .
                waveform = Waveforms::Sine,
                .volumeEnvelope = {
                    .numKeyFrames = 2,
                    .keyFrames = {
                        {
                            .sampleTime = 0,
                            .envelopeValue = 0.6
                        },
                        {
                            .sampleTime = 22100,
                            .envelopeValue = 0.2
                        }
                    }
                }
            }},
            {.oscillatorConfig = {.baseArgDelta = ((double)220 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine,
                    .volumeEnvelope = {
                        .numKeyFrames = 2,
                        .keyFrames = {
                            {
                                .sampleTime = 0,
                                .envelopeValue = 0
                            },
                            {
                                .sampleTime = 22100,
                                .envelopeValue = 0.05
                            }
                        }
                    }
            }},
            {.oscillatorConfig = {.baseArgDelta = ((double)5 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine,
                    .volumeEnvelope = {
                        .numKeyFrames = 2,
                        .keyFrames = {
                            {
                                .sampleTime = 0,
                                .envelopeValue = 0.001
                            },
                            {
                                .sampleTime = 22100,
                                .envelopeValue = 0
                            }
                        }
                    }
            }},
            {.oscillatorConfig = {.baseArgDelta = ((double)2 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine,
                    .volumeEnvelope = {
                        .numKeyFrames = 2,
                        .keyFrames = {
                            {
                                .sampleTime = 0,
                                .envelopeValue = 0.006
                            },
                            {
                                .sampleTime = 22100,
                                .envelopeValue = 0
                            }
                        }
                    }
            }},
            {.oscillatorConfig = {.baseArgDelta = ((double)210 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine,
                    .volumeEnvelope = {
                        .numKeyFrames = 2,
                        .keyFrames = {
                            {
                                .sampleTime = 0,
                                .envelopeValue = 0.01
                            },
                            {
                                .sampleTime = 22100,
                                .envelopeValue = 0.05
                            }
                        }
                    }
            }},
            {.oscillatorConfig = {.baseArgDelta = ((double)21 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine,
                    .volumeEnvelope = {
                        .numKeyFrames = 2,
                        .keyFrames = {
                            {
                                .sampleTime = 0,
                                .envelopeValue = 0.01
                            },
                            {
                                .sampleTime = 22100,
                                .envelopeValue = 0.05
                            }
                        }
                    }
            }},
        }};

    ToneGenerator<0>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<1>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<2>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<3>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<4>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<5>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<6>::AlgorithmTwo::loadConfig(config);
    ToneGenerator<7>::AlgorithmTwo::loadConfig(config);
}
void SyEngine::renderSamples(int16_t *buffer, int numChannels, int numSamples)
{
    int bufferCursor = 0;
    while (numSamples)
    {
        int samplesInLfoFrame = (LFO_FRAME_SAMPLE_COUNT - (_sampleTime & LFO_FRAME_BITMASK));
        samplesInLfoFrame = samplesInLfoFrame > numSamples ? numSamples : samplesInLfoFrame;
        _sampleTime += samplesInLfoFrame;
        numSamples -= samplesInLfoFrame;
        while (samplesInLfoFrame--)
        {
            auto sampleValue = ToneGenerator<0>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<1>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<2>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<3>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<4>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<5>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<6>::AlgorithmTwo::computeSample(1).getScaledValue() +
            ToneGenerator<7>::AlgorithmTwo::computeSample(1).getScaledValue();
            
            for (int channel = 0; channel < numChannels; channel++)
            {
                buffer[bufferCursor++] = sampleValue >> 16;
            }
        }
        if (!(_sampleTime & LFO_FRAME_BITMASK))
        {
            ToneGenerator<0>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<1>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<2>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<3>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<4>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<5>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<6>::AlgorithmTwo::updateEnvelope(_sampleTime);
            ToneGenerator<7>::AlgorithmTwo::updateEnvelope(_sampleTime);
        }
    }
}
