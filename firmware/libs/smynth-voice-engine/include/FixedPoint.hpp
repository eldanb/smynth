//
//  FixedPoint.hpp
//  SmynthVoiceEngine
//
//  Created by Eldan Ben-Haim on 13/02/2024.
//

#pragma once
#include <stdlib.h>
template <int FractBits, class UnderlyingIntegral = int32_t>
class FixedPoint
{
public:
    static constexpr const UnderlyingIntegral ScalingFactor = (1 << FractBits);
    static constexpr const UnderlyingIntegral FractBitMask = (1 << FractBits) - 1;

    FixedPoint()
        : FixedPoint(0)
    {
    }

    FixedPoint(double d) : FixedPoint((UnderlyingIntegral)(d * ScalingFactor))
    {
    }

    template <int OFractBits, class OUnderlyingIntegral>
    FixedPoint operator*(const FixedPoint<OFractBits, OUnderlyingIntegral> other) const
    {
        return FixedPoint(
            (_value * (other._value >> OFractBits)) +
            ((_value >> OFractBits) * (other._value & FixedPoint<OFractBits, OUnderlyingIntegral>::FractBitMask)));
    }

    FixedPoint operator*(const int32_t other) const
    {
        return FixedPoint((_value * other));
    }

    FixedPoint operator/(const int32_t other) const
    {
        return FixedPoint((_value / other));
    }

    FixedPoint operator+(const FixedPoint other) const
    {
        return FixedPoint(_value + other._value);
    }

    FixedPoint operator-(const FixedPoint other) const
    {
        return FixedPoint(_value - other._value);
    }

    FixedPoint operator>>(int shiftAmount) const
    {
        return FixedPoint(_value >> shiftAmount);
    }

    void maskIntegralPort(int mask)
    {
        _value = _value & ((mask << FractBits) | FractBitMask);
    }

    operator int()
    {
        return _value >> FractBits;
    }

    UnderlyingIntegral getScaledValue() const
    {
        return _value;
    }

private:
    explicit FixedPoint(UnderlyingIntegral value) : _value(value){};

    UnderlyingIntegral _value;
};


class fFixedPoint
{
public:
    
    fFixedPoint()
        : fFixedPoint(0)
    {
    }

    fFixedPoint(double d) : _value(d)
    {
    }

    fFixedPoint operator*(const fFixedPoint other) const
    {
        return fFixedPoint(_value *other._value);
    }

    fFixedPoint operator*(const int32_t other) const
    {
        return fFixedPoint((_value * other));
    }

    fFixedPoint operator/(const int32_t other) const
    {
        return fFixedPoint((_value / other));
    }

    fFixedPoint operator+(const fFixedPoint other) const
    {
        return fFixedPoint(_value + other._value);
    }

    fFixedPoint operator-(const fFixedPoint other) const
    {
        return fFixedPoint(_value - other._value);
    }

    fFixedPoint operator>>(int shiftAmount) const
    {
        return fFixedPoint(_value / (1 << shiftAmount));
    }

    void maskIntegralPort(int mask)
    {
        int integral = (int)_value;
        
        _value = (integral & mask) + (_value - integral);
    }

    operator int()
    {
        return (int)_value;
    }

    int getScaledValue() const
    {
        return _value * 65536;
    }

private:

    float _value;
};

using SampleValue = FixedPoint<16>;
//using SampleValue = fFixedPoint;


