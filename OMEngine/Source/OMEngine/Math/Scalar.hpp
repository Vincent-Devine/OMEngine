#pragma once

#include "OMEngine/Math/Common.hpp"

namespace OM::Math
{
    class Scalar
    {
    public:
        __forceinline Scalar() {}
        __forceinline Scalar(const Scalar& s) { _vec = s; }
        __forceinline Scalar(float f) { _vec = XMVectorReplicate(f); }
        __forceinline explicit Scalar(FXMVECTOR vec) { _vec = vec; }
        __forceinline explicit Scalar(EZeroTag) { _vec = SplatZero(); }
        __forceinline explicit Scalar(EIdentityTag) { _vec = SplatOne(); }

        __forceinline operator XMVECTOR() const { return _vec; }
        __forceinline operator float() const { return XMVectorGetX(_vec); }

    private:
        XMVECTOR _vec;
    };

    __forceinline Scalar operator- (Scalar s) { return Scalar(XMVectorNegate(s)); }
    __forceinline Scalar operator+ (Scalar s1, Scalar s2) { return Scalar(XMVectorAdd(s1, s2)); }
    __forceinline Scalar operator- (Scalar s1, Scalar s2) { return Scalar(XMVectorSubtract(s1, s2)); }
    __forceinline Scalar operator* (Scalar s1, Scalar s2) { return Scalar(XMVectorMultiply(s1, s2)); }
    __forceinline Scalar operator/ (Scalar s1, Scalar s2) { return Scalar(XMVectorDivide(s1, s2)); }
    __forceinline Scalar operator+ (Scalar s1, float s2) { return s1 + Scalar(s2); }
    __forceinline Scalar operator- (Scalar s1, float s2) { return s1 - Scalar(s2); }
    __forceinline Scalar operator* (Scalar s1, float s2) { return s1 * Scalar(s2); }
    __forceinline Scalar operator/ (Scalar s1, float s2) { return s1 / Scalar(s2); }
    __forceinline Scalar operator+ (float s1, Scalar s2) { return Scalar(s1) + s2; }
    __forceinline Scalar operator- (float s1, Scalar s2) { return Scalar(s1) - s2; }
    __forceinline Scalar operator* (float s1, Scalar s2) { return Scalar(s1) * s2; }
    __forceinline Scalar operator/ (float s1, Scalar s2) { return Scalar(s1) / s2; }
}