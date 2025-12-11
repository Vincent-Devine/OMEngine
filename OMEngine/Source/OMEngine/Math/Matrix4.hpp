#pragma once

#include "OMEngine/Math/Transform.hpp"

namespace OM::Math
{
    __declspec(align(16)) class Matrix4
    {
    public:
        _forceinline Matrix4() {}
        _forceinline Matrix4(Vector3 x, Vector3 y, Vector3 z, Vector3 w)
        {
            _mat.r[0] = SetWToZero(x); _mat.r[1] = SetWToZero(y);
            _mat.r[2] = SetWToZero(z); _mat.r[3] = SetWToOne(w);
        }

        _forceinline Matrix4(const float* data)
        {
            _mat = XMLoadFloat4x4((XMFLOAT4X4*)data);
        }

        _forceinline Matrix4(Vector4 x, Vector4 y, Vector4 z, Vector4 w) { _mat.r[0] = x; _mat.r[1] = y; _mat.r[2] = z; _mat.r[3] = w; }
        _forceinline Matrix4(const Matrix4& mat) { _mat = mat._mat; }
        _forceinline Matrix4(const Matrix3& mat)
        {
            _mat.r[0] = SetWToZero(mat.GetX());
            _mat.r[1] = SetWToZero(mat.GetY());
            _mat.r[2] = SetWToZero(mat.GetZ());
            _mat.r[3] = CreateWUnitVector();
        }
        _forceinline Matrix4(const Matrix3& xyz, Vector3 w)
        {
            _mat.r[0] = SetWToZero(xyz.GetX());
            _mat.r[1] = SetWToZero(xyz.GetY());
            _mat.r[2] = SetWToZero(xyz.GetZ());
            _mat.r[3] = SetWToOne(w);
        }
        _forceinline Matrix4(const AffineTransform& xform) { *this = Matrix4(xform.GetBasis(), xform.GetTranslation()); }
        _forceinline Matrix4(const OrthogonalTransform& xform) { *this = Matrix4(Matrix3(xform.GetRotation()), xform.GetTranslation()); }
        _forceinline explicit Matrix4(const XMMATRIX& mat) { _mat = mat; }
        _forceinline explicit Matrix4(EIdentityTag) { _mat = XMMatrixIdentity(); }
        _forceinline explicit Matrix4(EZeroTag) { _mat.r[0] = _mat.r[1] = _mat.r[2] = _mat.r[3] = SplatZero(); }

        _forceinline const Matrix3& Get3x3() const { return (const Matrix3&)*this; }
        _forceinline void Set3x3(const Matrix3& xyz)
        {
            _mat.r[0] = SetWToZero(xyz.GetX());
            _mat.r[1] = SetWToZero(xyz.GetY());
            _mat.r[2] = SetWToZero(xyz.GetZ());
        }

        _forceinline Vector4 GetX() const { return Vector4(_mat.r[0]); }
        _forceinline Vector4 GetY() const { return Vector4(_mat.r[1]); }
        _forceinline Vector4 GetZ() const { return Vector4(_mat.r[2]); }
        _forceinline Vector4 GetW() const { return Vector4(_mat.r[3]); }

        _forceinline void SetX(Vector4 x) { _mat.r[0] = x; }
        _forceinline void SetY(Vector4 y) { _mat.r[1] = y; }
        _forceinline void SetZ(Vector4 z) { _mat.r[2] = z; }
        _forceinline void SetW(Vector4 w) { _mat.r[3] = w; }

        _forceinline operator XMMATRIX() const { return _mat; }

        _forceinline Vector4 operator* (Vector3 vec) const { return Vector4(XMVector3Transform(vec, _mat)); }
        _forceinline Vector4 operator* (Vector4 vec) const { return Vector4(XMVector4Transform(vec, _mat)); }
        _forceinline Matrix4 operator* (const Matrix4& mat) const { return Matrix4(XMMatrixMultiply(mat, _mat)); }

        static _forceinline Matrix4 MakeScale(float scale) { return Matrix4(XMMatrixScaling(scale, scale, scale)); }
        static _forceinline Matrix4 MakeScale(Vector3 scale) { return Matrix4(XMMatrixScalingFromVector(scale)); }

    private:
        XMMATRIX _mat;
    };

}
