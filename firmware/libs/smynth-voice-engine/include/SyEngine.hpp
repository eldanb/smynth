#include <cstdint>


class SyEngine
{
public:
    SyEngine();

    void configureToneGenerator();
    void renderSamples(int16_t *buffers, int numChannels, int numSamples);

private:
    int32_t _sampleTime;
};
