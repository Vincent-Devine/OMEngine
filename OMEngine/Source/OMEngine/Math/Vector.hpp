#pragma once

#include "OMEngine/Math/Scalar.hpp"

namespace OM::Math
{
    class Vector4;

    // A 3-vector with an unspecified fourth component.  Depending on the context, the W can be 0 or 1, but both are implicit.
    // The actual value of the fourth component is undefined for performance reasons.
    class Vector3
    {
    public:

        __forceinline Vector3() {}
        __forceinline Vector3(float x, float y, float z) { _vec = XMVectorSet(x, y, z, z); }
        __forceinline Vector3(const XMFLOAT3& v) { _vec = XMLoadFloat3(&v); }
        __forceinline Vector3(const Vector3& v) { _vec = v; }
        __forceinline Vector3(Scalar s) { _vec = s; }
        __forceinline explicit Vector3(Vector4 vec);
        __forceinline explicit Vector3(FXMVECTOR vec) { _vec = vec; }
        __forceinline explicit Vector3(EZeroTag) { _vec = SplatZero(); }
        __forceinline explicit Vector3(EIdentityTag) { _vec = SplatOne(); }
        __forceinline explicit Vector3(EXUnitVector) { _vec = CreateXUnitVector(); }
        __forceinline explicit Vector3(EYUnitVector) { _vec = CreateYUnitVector(); }
        __forceinline explicit Vector3(EZUnitVector) { _vec = CreateZUnitVector(); }

        __forceinline operator XMVECTOR() const { return _vec; }

        __forceinline Scalar GetX() const { return Scalar(XMVectorSplatX(_vec)); }
        __forceinline Scalar GetY() const { return Scalar(XMVectorSplatY(_vec)); }
        __forceinline Scalar GetZ() const { return Scalar(XMVectorSplatZ(_vec)); }
        __forceinline void SetX(Scalar x) { _vec = XMVectorPermute<4, 1, 2, 3>(_vec, x); }
        __forceinline void SetY(Scalar y) { _vec = XMVectorPermute<0, 5, 2, 3>(_vec, y); }
        __forceinline void SetZ(Scalar z) { _vec = XMVectorPermute<0, 1, 6, 3>(_vec, z); }

        __forceinline Vector3 operator- () const { return Vector3(XMVectorNegate(_vec)); }
        __forceinline Vector3 operator+ (Vector3 v2) const { return Vector3(XMVectorAdd(_vec, v2)); }
        __forceinline Vector3 operator- (Vector3 v2) const { return Vector3(XMVectorSubtract(_vec, v2)); }
        __forceinline Vector3 operator* (Vector3 v2) const { return Vector3(XMVectorMultiply(_vec, v2)); }
        __forceinline Vector3 operator/ (Vector3 v2) const { return Vector3(XMVectorDivide(_vec, v2)); }
        __forceinline Vector3 operator* (Scalar  v2) const { return *this * Vector3(v2); }
        __forceinline Vector3 operator/ (Scalar  v2) const { return *this / Vector3(v2); }
        __forceinline Vector3 operator* (float  v2) const { return *this * Scalar(v2); }
        __forceinline Vector3 operator/ (float  v2) const { return *this / Scalar(v2); }

        __forceinline Vector3& operator += (Vector3 v) { *this = *this + v; return *this; }
        __forceinline Vector3& operator -= (Vector3 v) { *this = *this - v; return *this; }
        __forceinline Vector3& operator *= (Vector3 v) { *this = *this * v; return *this; }
        __forceinline Vector3& operator /= (Vector3 v) { *this = *this / v; return *this; }

        __forceinline friend Vector3 operator* (Scalar  v1, Vector3 v2) { return Vector3(v1) * v2; }
        __forceinline friend Vector3 operator/ (Scalar  v1, Vector3 v2) { return Vector3(v1) / v2; }
        __forceinline friend Vector3 operator* (float   v1, Vector3 v2) { return Scalar(v1) * v2; }
        __forceinline friend Vector3 operator/ (float   v1, Vector3 v2) { return Scalar(v1) / v2; }

    protected:
        XMVECTOR _vec;
    };

    // A 4-vector, completely defined.
    class Vector4
    {
    public:
        __forceinline Vector4() {}
        __forceinline Vector4(float x, float y, float z, float w) { _vec = XMVectorSet(x, y, z, w); }
        __forceinline Vector4(const XMFLOAT4& v) { _vec = XMLoadFloat4(&v); }
        __forceinline Vector4(Vector3 xyz, float w) { _vec = XMVectorSetW(xyz, w); }
        __forceinline Vector4(const Vector4& v) { _vec = v; }
        __forceinline Vector4(const Scalar& s) { _vec = s; }
        __forceinline explicit Vector4(Vector3 xyz) { _vec = SetWToOne(xyz); }
        __forceinline explicit Vector4(FXMVECTOR vec) { _vec = vec; }
        __forceinline explicit Vector4(EZeroTag) { _vec = SplatZero(); }
        __forceinline explicit Vector4(EIdentityTag) { _vec = SplatOne(); }
        __forceinline explicit Vector4(EXUnitVector) { _vec = CreateXUnitVector(); }
        __forceinline explicit Vector4(EYUnitVector) { _vec = CreateYUnitVector(); }
        __forceinline explicit Vector4(EZUnitVector) { _vec = CreateZUnitVector(); }
        __forceinline explicit Vector4(EWUnitVector) { _vec = CreateWUnitVector(); }

        __forceinline operator XMVECTOR() const { return _vec; }

        __forceinline Scalar GetX() const { return Scalar(XMVectorSplatX(_vec)); }
        __forceinline Scalar GetY() const { return Scalar(XMVectorSplatY(_vec)); }
        __forceinline Scalar GetZ() const { return Scalar(XMVectorSplatZ(_vec)); }
        __forceinline Scalar GetW() const { return Scalar(XMVectorSplatW(_vec)); }
        __forceinline void SetX(Scalar x) { _vec = XMVectorPermute<4, 1, 2, 3>(_vec, x); }
        __forceinline void SetY(Scalar y) { _vec = XMVectorPermute<0, 5, 2, 3>(_vec, y); }
        __forceinline void SetZ(Scalar z) { _vec = XMVectorPermute<0, 1, 6, 3>(_vec, z); }
        __forceinline void SetW(Scalar w) { _vec = XMVectorPermute<0, 1, 2, 7>(_vec, w); }
        __forceinline void SetXYZ(Vector3 xyz) { _vec = XMVectorPermute<0, 1, 2, 7>(xyz, _vec); }

        __forceinline Vector4 operator- () const { return Vector4(XMVectorNegate(_vec)); }
        __forceinline Vector4 operator+ (Vector4 v2) const { return Vector4(XMVectorAdd(_vec, v2)); }
        __forceinline Vector4 operator- (Vector4 v2) const { return Vector4(XMVectorSubtract(_vec, v2)); }
        __forceinline Vector4 operator* (Vector4 v2) const { return Vector4(XMVectorMultiply(_vec, v2)); }
        __forceinline Vector4 operator/ (Vector4 v2) const { return Vector4(XMVectorDivide(_vec, v2)); }
        __forceinline Vector4 operator* (Scalar  v2) const { return *this * Vector4(v2); }
        __forceinline Vector4 operator/ (Scalar  v2) const { return *this / Vector4(v2); }
        __forceinline Vector4 operator* (float   v2) const { return *this * Scalar(v2); }
        __forceinline Vector4 operator/ (float   v2) const { return *this / Scalar(v2); }

        __forceinline void operator*= (float   v2) { *this = *this * Scalar(v2); }
        __forceinline void operator/= (float   v2) { *this = *this / Scalar(v2); }

        __forceinline friend Vector4 operator* (Scalar  v1, Vector4 v2) { return Vector4(v1) * v2; }
        __forceinline friend Vector4 operator/ (Scalar  v1, Vector4 v2) { return Vector4(v1) / v2; }
        __forceinline friend Vector4 operator* (float   v1, Vector4 v2) { return Scalar(v1) * v2; }
        __forceinline friend Vector4 operator/ (float   v1, Vector4 v2) { return Scalar(v1) / v2; }

    protected:
        XMVECTOR _vec;
    };

    // Defined after Vector4 methods are declared
    __forceinline Vector3::Vector3(Vector4 vec) : _vec((XMVECTOR)vec)
    {
    }

    // For W != 1, divide XYZ by W.  If W == 0, do nothing
    __forceinline Vector3 MakeHomogeneous(Vector4 v)
    {
        Scalar W = v.GetW();
        return Vector3(XMVectorSelect(XMVectorDivide(v, W), v, XMVectorEqual(W, SplatZero())));
    }

    class BoolVector
    {
    public:
        __forceinline BoolVector(FXMVECTOR vec) { _vec = vec; }
        __forceinline operator XMVECTOR() const { return _vec; }
    protected:
        XMVECTOR _vec;
    };
}