#include "SyEngine.hpp"
#include <math.h>

#include "FixedPoint.hpp"
#include "Waveforms.hpp"
#include "ToneGenerator.hpp"
#include "Effect.hpp"

#define SAMPLING_RATE 44100

ReverbFilter reverbFilter;

SyEngine::SyEngine()
{
    configureToneGenerator();
}

#define K1 0.9
void SyEngine::configureToneGenerator()
{
    ToneGeneratorConfig config = {
        .nodeConfig = {
            {.oscillatorConfig = {
                 .baseArgDelta = ((double)220 * 2 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT,
                 .waveform = Waveforms::Sine,
                 .volumeEnvelope = {
                     .numKeyFrames = 4,
                     .keyFrames = {
                         {.sampleTime = _sampleTime + 0,
                          .envelopeValue = 0.03 * 0.92},
                         {.sampleTime = _sampleTime + 22050,
                          .envelopeValue = 0.03 * 0.7},
                         {.sampleTime = _sampleTime + (int)(1.9 * SAMPLING_RATE),
                          .envelopeValue = 0.03 * 0.7},
                         {.sampleTime = _sampleTime + (int)(2 * SAMPLING_RATE),
                          .envelopeValue = 0}}}}},
            {.oscillatorConfig = {.baseArgDelta = ((double)220 * 2.1 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine, .volumeEnvelope = {.numKeyFrames = 10, .keyFrames = {{.sampleTime = _sampleTime + 0, .envelopeValue = 0}, {.sampleTime = _sampleTime + (int)(0.2 * SAMPLING_RATE), .envelopeValue = K1 * 1}, {.sampleTime = _sampleTime + (int)(0.25 * SAMPLING_RATE), .envelopeValue = K1 * 0.1}, {.sampleTime = _sampleTime + (int)(0.4 * SAMPLING_RATE), .envelopeValue = K1 * 0.16}, {.sampleTime = _sampleTime + (int)(0.61 * SAMPLING_RATE), .envelopeValue = K1 * 0.75}, {.sampleTime = _sampleTime + (int)(0.75 * SAMPLING_RATE), .envelopeValue = K1 * 0.18}, {.sampleTime = _sampleTime + (int)(1 * SAMPLING_RATE), .envelopeValue = K1 * 0.2}, {.sampleTime = _sampleTime + (int)(1.1 * SAMPLING_RATE), .envelopeValue = K1 * 0.55}, {.sampleTime = _sampleTime + (int)(1.25 * SAMPLING_RATE), .envelopeValue = K1 * 0.16}, {.sampleTime = _sampleTime + (int)(2.7 * SAMPLING_RATE), .envelopeValue = K1 * 0.35}}}}},
            {.oscillatorConfig = {.baseArgDelta = ((double)220 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine, .volumeEnvelope = {.numKeyFrames = 2, .keyFrames = {{.sampleTime = _sampleTime + 0, .envelopeValue = 0}, {.sampleTime = _sampleTime + 22100, .envelopeValue = 0.05}}}}},
            {.oscillatorConfig = {.baseArgDelta = ((double)5 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine, .volumeEnvelope = {.numKeyFrames = 2, .keyFrames = {{.sampleTime = _sampleTime + 0, .envelopeValue = 0.001}, {.sampleTime = _sampleTime + 22100, .envelopeValue = 0}}}}},
            {.oscillatorConfig = {.baseArgDelta = ((double)2 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine, .volumeEnvelope = {.numKeyFrames = 2, .keyFrames = {{.sampleTime = _sampleTime + 0, .envelopeValue = 0.006}, {.sampleTime = _sampleTime + 22100, .envelopeValue = 0}}}}},
            {.oscillatorConfig = {.baseArgDelta = ((double)210 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine, .volumeEnvelope = {.numKeyFrames = 2, .keyFrames = {{.sampleTime = _sampleTime + 0, .envelopeValue = 0.01}, {.sampleTime = _sampleTime + 22100, .envelopeValue = 0.05}}}}},
            {.oscillatorConfig = {.baseArgDelta = ((double)21 / SAMPLING_RATE) * WAVEFORM_LOOKUPTABLE_COUNT, .waveform = Waveforms::Sine, .volumeEnvelope = {.numKeyFrames = 2, .keyFrames = {{.sampleTime = _sampleTime + 0, .envelopeValue = 0.01}, {.sampleTime = _sampleTime + 22100, .envelopeValue = 0.05}}}}},
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
            auto sampleValue = ToneGenerator<0>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<1>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<2>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<3>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<4>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<5>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<6>::AlgorithmTwo::computeSample(1) +
                               ToneGenerator<7>::AlgorithmTwo::computeSample(1);
            sampleValue = reverbFilter.apply(sampleValue);
            for (int channel = 0; channel < numChannels; channel++)
            {
                buffer[bufferCursor++] = sampleValue.getScaledValue() >> 16;
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
