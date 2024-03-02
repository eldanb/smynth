#include "FixedPoint.hpp"
#include <vector>

inline int32_t clampTo16BitSample(int32_t value)
{
  value = (value < -32767 ? -32767 : value);
  value = (value > 32767) ? 32767 : value;
  return value;
}

class CombFilter
{
public:
  CombFilter()
  {
    setDelayAndGain(1, 0);
  }

  int32_t apply(int32_t sampleValue)
  {
    int32_t ret = clampTo16BitSample(sampleValue + _gain * _samples[_cursor]);

    _samples[_cursor] = ret;
    _cursor = (_cursor >= _delay - 1) ? 0 : (_cursor + 1);
    return ret;
  }

  void setDelayAndGain(int delay, SampleValue gain)
  {
    _samples.resize(delay);
    _delay = delay;
    _gain = gain;
    _cursor = 0;
  }

private:
  std::vector<int16_t> _samples;
  SampleValue _gain;
  int _delay;
  int _cursor;
};

class AllPassFilter
{
public:
  AllPassFilter()
  {
    setDelayAndGain(1, 0);
  }

  int32_t apply(int32_t sampleValue)
  {
    int32_t ret = clampTo16BitSample(((_outputs[_cursor] - sampleValue) * _gain) + _inputs[_cursor]);

    _inputs[_cursor] = sampleValue;
    _outputs[_cursor] = ret;
    _cursor = (_cursor >= _delay - 1) ? 0 : (_cursor + 1);
    return ret;
  }

  void setDelayAndGain(int delay, SampleValue gain)
  {
    _inputs.resize(delay);
    _outputs.resize(delay);
    _delay = delay;
    _gain = gain;
    _cursor = 0;
  }

private:
  std::vector<int16_t> _inputs;
  std::vector<int16_t> _outputs;
  SampleValue _gain;
  int _delay;
  int _cursor;
};

class ReverbFilter
{
public:
  ReverbFilter()
      : _reverbMix(0.2)
  {
    _comb1.setDelayAndGain(0.1 * 44100, 0.742);
    _comb2.setDelayAndGain(0.102 * 44100, 0.733);
    _comb3.setDelayAndGain(0.104 * 44100, 0.715);
    _comb4.setDelayAndGain(0.12 * 44100, 0.697);
    _allPass1.setDelayAndGain(0.025 * 44100, 0.7);
    _allPass2.setDelayAndGain(0.007 * 44100, 0.7);
  }

  inline SampleValue apply(SampleValue sampleValue)
  {
    int32_t combOutput = clampTo16BitSample(_comb1.apply(sampleValue) + _comb2.apply(sampleValue) + _comb3.apply(sampleValue) + _comb4.apply(sampleValue));
    int32_t allPass1Output = _allPass1.apply(combOutput);
    int32_t allPass2Output = _allPass2.apply(allPass1Output);
    return clampTo16BitSample(sampleValue + _reverbMix * allPass2Output);
  }

private:
  CombFilter _comb1, _comb2, _comb3, _comb4;
  AllPassFilter _allPass1, _allPass2;
  SampleValue _reverbMix;
};
