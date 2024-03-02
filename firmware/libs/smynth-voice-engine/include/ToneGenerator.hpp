//
//  ToneGenerator.hpp
//  SmynthVoiceEngine
//
//  Created by Eldan Ben-Haim on 13/02/2024.
//

#pragma once

#include "FixedPoint.hpp"
#include "Waveforms.hpp"

#define LFO_FRAME_BITCOUNT 7
#define LFO_FRAME_SAMPLE_COUNT (1 << LFO_FRAME_BITCOUNT)
#define LFO_FRAME_BITMASK ((1 << LFO_FRAME_BITCOUNT) - 1)

#define MAX_ENVELOPE_KEYFRAMES 16

struct EnvelopeKeyFrame
{
    int32_t sampleTime;
    SampleValue envelopeValue;
};

struct EnvelopeConfig
{
    int numKeyFrames;
    EnvelopeKeyFrame keyFrames[MAX_ENVELOPE_KEYFRAMES];
};

union NodeConfig
{
    struct
    {
        SampleValue baseArgDelta;
        WaveFormLookupTable waveform;
        EnvelopeConfig volumeEnvelope;
    } oscillatorConfig;
};

struct ToneGeneratorConfig
{
    NodeConfig nodeConfig[8];
};

template <int GENEARTOR>
class ToneGenerator
{
public:
    template <int ENVELPE_GENERATOR_ID>
    class EnvelopeGenerator
    {
    public:
        static SampleValue generateNextEnvelopeFrame(int32_t sampleTime)
        {
            if (nextEnvelopeSampleTime < 0)
            {
                return current;
            }

            if (sampleTime < nextEnvelopeSampleTime)
            {
                current = current + deltaPerLfoFrame;
                return current;
            }

            envelopeKeyFrameIndex++;
            loadKeyframe();
            return current;
        }

        static SampleValue loadConfiguration(const EnvelopeConfig &config)
        {
            envelopeConfig = config;
            envelopeKeyFrameIndex = 0;
            loadKeyframe();
            return current;
        }

    private:
        static void loadKeyframe()
        {
            current = envelopeConfig.keyFrames[envelopeKeyFrameIndex].envelopeValue;
            if (envelopeKeyFrameIndex < envelopeConfig.numKeyFrames - 1)
            {
                nextEnvelopeSampleTime = envelopeConfig.keyFrames[envelopeKeyFrameIndex + 1].sampleTime;

                auto deltaEnvelope = (envelopeConfig.keyFrames[envelopeKeyFrameIndex + 1].envelopeValue - envelopeConfig.keyFrames[envelopeKeyFrameIndex].envelopeValue);
                auto deltaTime = (envelopeConfig.keyFrames[envelopeKeyFrameIndex + 1].sampleTime - envelopeConfig.keyFrames[envelopeKeyFrameIndex].sampleTime);

                deltaPerLfoFrame = deltaEnvelope * 128 / deltaTime;
            }
            else
            {
                nextEnvelopeSampleTime = -1;
            }
        }

    private:
        static SampleValue current;
        static SampleValue deltaPerLfoFrame;
        static int32_t nextEnvelopeSampleTime;
        static int envelopeKeyFrameIndex;

        static EnvelopeConfig envelopeConfig;
    };

    template <int NODE_INDEX>
    struct NodeState
    {
        static WaveFormLookupTable waveform;
        static SampleValue baseArgDelta;
        static SampleValue scale;

        static SampleValue lastSampleIndex;
    };

    template <int NODE_INDEX, class MODULATING_OSCILLATOR>
    class ModulatedOscilator
    {
        using LocalNodeState = NodeState<NODE_INDEX>;
        using LocalEnvelopeGenerator = EnvelopeGenerator<NODE_INDEX>;

    public:
        static inline SampleValue computeSample(int skipCount)
        {
            auto modulatingSample = MODULATING_OSCILLATOR::computeSample(skipCount);
            LocalNodeState::lastSampleIndex = (LocalNodeState::lastSampleIndex + (LocalNodeState::baseArgDelta + (modulatingSample >> 7)));

            LocalNodeState::lastSampleIndex.maskIntegralPort(WAVEFORM_LOOKUPTABLE_COUNT - 1);

            auto scale = LocalNodeState::scale;
            return LocalNodeState::waveform[LocalNodeState::lastSampleIndex] * LocalNodeState::scale;
        }

        static inline void updateEnvelope(int32_t sampleTime)
        {
            LocalNodeState::scale = LocalEnvelopeGenerator::generateNextEnvelopeFrame(sampleTime);
            MODULATING_OSCILLATOR::updateEnvelope(sampleTime);
        }

        static inline void loadConfig(const ToneGeneratorConfig &config)
        {
            LocalNodeState::baseArgDelta = config.nodeConfig[NODE_INDEX].oscillatorConfig.baseArgDelta;
            LocalNodeState::waveform = config.nodeConfig[NODE_INDEX].oscillatorConfig.waveform;
            LocalNodeState::scale = LocalEnvelopeGenerator::loadConfiguration(config.nodeConfig[NODE_INDEX].oscillatorConfig.volumeEnvelope);
            MODULATING_OSCILLATOR::loadConfig(config);
        };
    };

    template <int NODE_INDEX>
    class NonModulatedOscilator
    {
        using LocalNodeState = NodeState<NODE_INDEX>;
        using LocalEnvelopeGenerator = EnvelopeGenerator<NODE_INDEX>;

    public:
        static inline SampleValue computeSample(int skipCount)
        {
            LocalNodeState::lastSampleIndex = (LocalNodeState::lastSampleIndex + LocalNodeState::baseArgDelta);
            LocalNodeState::lastSampleIndex.maskIntegralPort(WAVEFORM_LOOKUPTABLE_COUNT - 1);

            return LocalNodeState::waveform[LocalNodeState::lastSampleIndex] * LocalNodeState::scale;
        }

        static inline void updateEnvelope(int32_t sampleTime)
        {
            LocalNodeState::scale = LocalEnvelopeGenerator::generateNextEnvelopeFrame(sampleTime);
        }

        static inline void loadConfig(const ToneGeneratorConfig &config)
        {
            LocalNodeState::baseArgDelta = config.nodeConfig[NODE_INDEX].oscillatorConfig.baseArgDelta;
            LocalNodeState::waveform = config.nodeConfig[NODE_INDEX].oscillatorConfig.waveform;
            LocalEnvelopeGenerator::loadConfiguration(config.nodeConfig[NODE_INDEX].oscillatorConfig.volumeEnvelope);
        };
    };

    template <class INPUT1, class INPUT2>
    class Mixer
    {
    public:
        static inline SampleValue computeSample(int skipCount)
        {
            auto sample1 = INPUT1::computeSample(skipCount);
            auto sample2 = INPUT2::computeSample(skipCount);

            return sample1 + sample2;
        }

        static inline void updateEnvelope(int32_t sampleTime)
        {
            INPUT1::updateEnvelope(sampleTime);
            INPUT2::updateEnvelope(sampleTime);
        }

        static inline void loadConfig(const ToneGeneratorConfig &config)
        {
            INPUT1::loadConfig(config);
            INPUT2::loadConfig(config);
        };

    private:
    };

    using AlgorithmOne = ModulatedOscilator<0, NonModulatedOscilator<1>>;

    /*using AlgorithmTwo = ModulatedOscilator<0, ModulatedOscilator<1, Mixer<2, NonModulatedOscilator<3>, NonModulatedOscilator<4>>>>;*/
    /*using AlgorithmTwo = Mixer<
        ModulatedOscilator<0,
                           ModulatedOscilator<1,
                                              Mixer<NonModulatedOscilator<3>, NonModulatedOscilator<4>>>>,
        ModulatedOscilator<2,
                           Mixer<NonModulatedOscilator<5>, NonModulatedOscilator<6>>>>;*/
    using AlgorithmTwo = ModulatedOscilator<0, NonModulatedOscilator<1>>;
    // using AlgorithmTwo = NonModulatedOscilator<0>;
};

template <int GENERATOR>
template <int NODE_INDEX>
SampleValue ToneGenerator<GENERATOR>::NodeState<NODE_INDEX>::lastSampleIndex;

template <int GENERATOR>
template <int NODE_INDEX>
SampleValue ToneGenerator<GENERATOR>::NodeState<NODE_INDEX>::baseArgDelta;

template <int GENERATOR>
template <int NODE_INDEX>
SampleValue ToneGenerator<GENERATOR>::NodeState<NODE_INDEX>::scale;

template <int GENERATOR>
template <int NODE_INDEX>
WaveFormLookupTable ToneGenerator<GENERATOR>::NodeState<NODE_INDEX>::waveform;

template <int GENERATOR>
template <int NODE_INDEX>
SampleValue ToneGenerator<GENERATOR>::EnvelopeGenerator<NODE_INDEX>::current;

template <int GENERATOR>
template <int NODE_INDEX>
SampleValue ToneGenerator<GENERATOR>::EnvelopeGenerator<NODE_INDEX>::deltaPerLfoFrame;

template <int GENERATOR>
template <int NODE_INDEX>
int32_t ToneGenerator<GENERATOR>::EnvelopeGenerator<NODE_INDEX>::nextEnvelopeSampleTime;

template <int GENERATOR>
template <int NODE_INDEX>
int ToneGenerator<GENERATOR>::EnvelopeGenerator<NODE_INDEX>::envelopeKeyFrameIndex;

template <int GENERATOR>
template <int NODE_INDEX>
EnvelopeConfig ToneGenerator<GENERATOR>::EnvelopeGenerator<NODE_INDEX>::envelopeConfig;
