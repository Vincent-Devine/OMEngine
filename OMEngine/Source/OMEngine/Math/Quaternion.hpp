#pragma once

#include "OMEngine/Math/Vector.hpp"

namespace OM::Math
{
    class Quaternion
    {
    public:
        __forceinline Quaternion() { _vec = XMQuaternionIdentity(); }
        __forceinline Quaternion(const Vector3& axis, const Scalar& angle) { _vec = XMQuaternionRotationAxis(axis, angle); }
        __forceinline Quaternion(float pitch, float yaw, float roll) { _vec = XMQuaternionRotationRollPitchYaw(pitch, yaw, roll); }
        __forceinline explicit Quaternion(const XMMATRIX& matrix) { _vec = XMQuaternionRotationMatrix(matrix); }
        __forceinline explicit Quaternion(FXMVECTOR vec) { _vec = vec; }
        __forceinline explicit Quaternion(EIdentityTag) { _vec = XMQuaternionIdentity(); }

        __forceinline operator XMVECTOR() const { return _vec; }

        __forceinline Quaternion operator~ (void) const { return Quaternion(XMQuaternionConjugate(_vec)); }
        __forceinline Quaternion operator- (void) const { return Quaternion(XMVectorNegate(_vec)); }

        __forceinline Quaternion operator* (Quaternion rhs) const { return Quaternion(XMQuaternionMultiply(rhs, _vec)); }
        __forceinline Vector3 operator* (Vector3 rhs) const { return Vector3(XMVector3Rotate(rhs, _vec)); }

        __forceinline Quaternion& operator= (Quaternion rhs) { _vec = rhs; return *this; }
        __forceinline Quaternion& operator*= (Quaternion rhs) { *this = *this * rhs; return *this; }

    protected:
        XMVECTOR _vec;
    };

    __forceinline Quaternion Normalize(Quaternion q) { return Quaternion(XMQuaternionNormalize(q)); }
    __forceinline Quaternion Slerp(Quaternion a, Quaternion b, float t) { return Normalize(Quaternion(XMQuaternionSlerp(a, b, t))); }
    __forceinline Quaternion Lerp(Quaternion a, Quaternion b, float t) { return Normalize(Quaternion(XMVectorLerp(a, b, t))); }
}