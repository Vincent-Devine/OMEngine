#pragma once

#include "OMEngine/Math/Quaternion.hpp"

namespace OM::Math
{
    // Represents a 3x3 matrix while occuping a 3x4 memory footprint.  The unused row and column are undefined but implicitly
    // (0, 0, 0, 1).  Constructing a Matrix4 will make those values explicit.
    __declspec(align(16)) class Matrix3
    {
    public:
        __forceinline Matrix3() {}
        __forceinline Matrix3(Vector3 x, Vector3 y, Vector3 z) { _mat[0] = x; _mat[1] = y; _mat[2] = z; }
        __forceinline Matrix3(const Matrix3& m) { _mat[0] = m._mat[0]; _mat[1] = m._mat[1]; _mat[2] = m._mat[2]; }
        __forceinline Matrix3(Quaternion q) { *this = Matrix3(XMMatrixRotationQuaternion(q)); }
        __forceinline explicit Matrix3(const XMMATRIX& m) { _mat[0] = Vector3(m.r[0]); _mat[1] = Vector3(m.r[1]); _mat[2] = Vector3(m.r[2]); }
        __forceinline explicit Matrix3(EIdentityTag) { _mat[0] = Vector3(kXUnitVector); _mat[1] = Vector3(kYUnitVector); _mat[2] = Vector3(kZUnitVector); }
        __forceinline explicit Matrix3(EZeroTag) { _mat[0] = _mat[1] = _mat[2] = Vector3(kZero); }

        __forceinline void SetX(Vector3 x) { _mat[0] = x; }
        __forceinline void SetY(Vector3 y) { _mat[1] = y; }
        __forceinline void SetZ(Vector3 z) { _mat[2] = z; }

        __forceinline Vector3 GetX() const { return _mat[0]; }
        __forceinline Vector3 GetY() const { return _mat[1]; }
        __forceinline Vector3 GetZ() const { return _mat[2]; }

        static __forceinline Matrix3 MakeXRotation(float angle) { return Matrix3(XMMatrixRotationX(angle)); }
        static __forceinline Matrix3 MakeYRotation(float angle) { return Matrix3(XMMatrixRotationY(angle)); }
        static __forceinline Matrix3 MakeZRotation(float angle) { return Matrix3(XMMatrixRotationZ(angle)); }
        static __forceinline Matrix3 MakeScale(float scale) { return Matrix3(XMMatrixScaling(scale, scale, scale)); }
        static __forceinline Matrix3 MakeScale(float sx, float sy, float sz) { return Matrix3(XMMatrixScaling(sx, sy, sz)); }
        static __forceinline Matrix3 MakeScale(const XMFLOAT3& scale) { return Matrix3(XMMatrixScaling(scale.x, scale.y, scale.z)); }
        static __forceinline Matrix3 MakeScale(Vector3 scale) { return Matrix3(XMMatrixScalingFromVector(scale)); }

        // Useful for DirectXMath interaction.  WARNING:  Only the 3x3 elements are defined.
        __forceinline operator XMMATRIX() const { return XMMATRIX(_mat[0], _mat[1], _mat[2], XMVectorZero()); }

        __forceinline Matrix3 operator* (Scalar scl) const { return Matrix3(scl * GetX(), scl * GetY(), scl * GetZ()); }
        __forceinline Vector3 operator* (Vector3 vec) const { return Vector3(XMVector3TransformNormal(vec, *this)); }
        __forceinline Matrix3 operator* (const Matrix3& mat) const { return Matrix3(*this * mat.GetX(), *this * mat.GetY(), *this * mat.GetZ()); }

    private:
        Vector3 _mat[3];
    };
}