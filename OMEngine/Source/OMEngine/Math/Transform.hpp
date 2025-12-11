#pragma once

#include "OMEngine/Math/Matrix3.hpp"
#include "OMEngine/Math/BoundingSphere.hpp"

namespace OM::Math
{
    class OrthogonalTransform;  // Orthonormal basis (just rotation via quaternion) and translation
    class AffineTransform;      // A 3x4 matrix that allows for asymmetric skew and scale
    class ScaleAndTranslation;  // Uniform scale and translation that can be compactly represented in a vec4
    class UniformTransform;     // Uniform scale, rotation (quaternion), and translation that fits in two vec4s

    class OrthogonalTransform   // This transform strictly prohibits non-uniform scale.  Scale itself is barely tolerated.
    {
    public:
        __forceinline OrthogonalTransform() : _rotation(kIdentity), _translation(kZero) {}
        __forceinline OrthogonalTransform(Quaternion rotate) : _rotation(rotate), _translation(kZero) {}
        __forceinline OrthogonalTransform(Vector3 translate) : _rotation(kIdentity), _translation(translate) {}
        __forceinline OrthogonalTransform(Quaternion rotate, Vector3 translate) : _rotation(rotate), _translation(translate) {}
        __forceinline OrthogonalTransform(const Matrix3& mat) : _rotation(mat), _translation(kZero) {}
        __forceinline OrthogonalTransform(const Matrix3& mat, Vector3 translate) : _rotation(mat), _translation(translate) {}
        __forceinline OrthogonalTransform(EIdentityTag) : _rotation(kIdentity), _translation(kZero) {}
        __forceinline explicit OrthogonalTransform(const XMMATRIX& mat) { *this = OrthogonalTransform(Matrix3(mat), Vector3(mat.r[3])); }

        __forceinline void SetRotation(Quaternion q) { _rotation = q; }
        __forceinline void SetTranslation(Vector3 v) { _translation = v; }

        __forceinline Quaternion GetRotation() const { return _rotation; }
        __forceinline Vector3 GetTranslation() const { return _translation; }

        static __forceinline OrthogonalTransform MakeXRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kXUnitVector), angle)); }
        static __forceinline OrthogonalTransform MakeYRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kYUnitVector), angle)); }
        static __forceinline OrthogonalTransform MakeZRotation(float angle) { return OrthogonalTransform(Quaternion(Vector3(kZUnitVector), angle)); }
        static __forceinline OrthogonalTransform MakeTranslation(Vector3 translate) { return OrthogonalTransform(translate); }

        __forceinline Vector3 operator* (Vector3 vec) const { return _rotation * vec + _translation; }
        __forceinline Vector4 operator* (Vector4 vec) const {
            return
                Vector4(SetWToZero(_rotation * Vector3((XMVECTOR)vec))) +
                Vector4(SetWToOne(_translation)) * vec.GetW();
        }
        __forceinline BoundingSphere operator* (BoundingSphere sphere) const {
            return BoundingSphere(*this * sphere.GetCenter(), sphere.GetRadius());
        }

        __forceinline OrthogonalTransform operator* (const OrthogonalTransform& xform) const {
            return OrthogonalTransform(_rotation * xform._rotation, _rotation * xform._translation + _translation);
        }

        __forceinline OrthogonalTransform operator~ () const {
            Quaternion invertedRotation = ~_rotation;
            return OrthogonalTransform(invertedRotation, invertedRotation * -_translation);
        }

    private:

        Quaternion _rotation;
        Vector3 _translation;
    };

    //
    // A transform that lacks rotation and has only uniform scale.
    //
    class ScaleAndTranslation
    {
    public:
        __forceinline ScaleAndTranslation()
        {
        }
        __forceinline ScaleAndTranslation(EIdentityTag)
            : _repr(kWUnitVector) {
        }
        __forceinline ScaleAndTranslation(float tx, float ty, float tz, float scale)
            : _repr(tx, ty, tz, scale) {
        }
        __forceinline ScaleAndTranslation(Vector3 translate, Scalar scale)
        {
            _repr = Vector4(translate);
            _repr.SetW(scale);
        }
        __forceinline explicit ScaleAndTranslation(const XMVECTOR& v)
            : _repr(v) {
        }

        __forceinline void SetScale(Scalar s) { _repr.SetW(s); }
        __forceinline void SetTranslation(Vector3 t) { _repr.SetXYZ(t); }

        __forceinline Scalar GetScale() const { return _repr.GetW(); }
        __forceinline Vector3 GetTranslation() const { return (Vector3)_repr; }

        __forceinline BoundingSphere operator*(const BoundingSphere& sphere) const
        {
            Vector4 scaledSphere = (Vector4)sphere * GetScale();
            Vector4 translation = Vector4(SetWToZero(_repr));
            return BoundingSphere(scaledSphere + translation);
        }

    private:
        Vector4 _repr;
    };

    //
    // This transform allows for rotation, translation, and uniform scale
    // 
    class UniformTransform
    {
    public:
        __forceinline UniformTransform()
        {
        }
        __forceinline UniformTransform(EIdentityTag)
            : _rotation(kIdentity), _translationScale(kIdentity) {
        }
        __forceinline UniformTransform(Quaternion rotation, ScaleAndTranslation transScale)
            : _rotation(rotation), _translationScale(transScale)
        {
        }
        __forceinline UniformTransform(Quaternion rotation, Scalar scale, Vector3 translation)
            : _rotation(rotation), _translationScale(translation, scale)
        {
        }

        __forceinline void SetRotation(Quaternion r) { _rotation = r; }
        __forceinline void SetScale(Scalar s) { _translationScale.SetScale(s); }
        __forceinline void SetTranslation(Vector3 t) { _translationScale.SetTranslation(t); }


        __forceinline Quaternion GetRotation() const { return _rotation; }
        __forceinline Scalar GetScale() const { return _translationScale.GetScale(); }
        __forceinline Vector3 GetTranslation() const { return _translationScale.GetTranslation(); }


        __forceinline Vector3 operator*(Vector3 vec) const
        {
            return _rotation * (vec * _translationScale.GetScale()) + _translationScale.GetTranslation();
        }

        __forceinline BoundingSphere operator*(BoundingSphere sphere) const
        {
            return BoundingSphere(*this * sphere.GetCenter(), GetScale() * sphere.GetRadius());
        }

    private:
        Quaternion _rotation;
        ScaleAndTranslation _translationScale;
    };

    // A AffineTransform is a 3x4 matrix with an implicit 4th row = [0,0,0,1].  This is used to perform a change of
    // basis on 3D points.  An affine transformation does not have to have orthonormal basis vectors.
    class AffineTransform
    {
    public:
        __forceinline AffineTransform()
        {
        }
        __forceinline AffineTransform(Vector3 x, Vector3 y, Vector3 z, Vector3 w)
            : _basis(x, y, z), _translation(w) {
        }
        __forceinline AffineTransform(Vector3 translate)
            : _basis(kIdentity), _translation(translate) {
        }
        __forceinline AffineTransform(const Matrix3& mat, Vector3 translate = Vector3(kZero))
            : _basis(mat), _translation(translate) {
        }
        __forceinline AffineTransform(Quaternion rot, Vector3 translate = Vector3(kZero))
            : _basis(rot), _translation(translate) {
        }
        __forceinline AffineTransform(const OrthogonalTransform& xform)
            : _basis(xform.GetRotation()), _translation(xform.GetTranslation()) {
        }
        __forceinline AffineTransform(const UniformTransform& xform)
        {
            _basis = Matrix3(xform.GetRotation()) * xform.GetScale();
            _translation = xform.GetTranslation();
        }
        __forceinline AffineTransform(EIdentityTag)
            : _basis(kIdentity), _translation(kZero) {
        }
        __forceinline explicit AffineTransform(const XMMATRIX& mat)
            : _basis(mat), _translation(mat.r[3]) {
        }

        __forceinline operator XMMATRIX() const { return (XMMATRIX&)*this; }

        __forceinline void SetX(Vector3 x) { _basis.SetX(x); }
        __forceinline void SetY(Vector3 y) { _basis.SetY(y); }
        __forceinline void SetZ(Vector3 z) { _basis.SetZ(z); }
        __forceinline void SetTranslation(Vector3 w) { _translation = w; }
        __forceinline void SetBasis(const Matrix3& basis) { _basis = basis; }

        __forceinline Vector3 GetX() const { return _basis.GetX(); }
        __forceinline Vector3 GetY() const { return _basis.GetY(); }
        __forceinline Vector3 GetZ() const { return _basis.GetZ(); }
        __forceinline Vector3 GetTranslation() const { return _translation; }
        __forceinline const Matrix3& GetBasis() const { return (const Matrix3&)*this; }

        static __forceinline AffineTransform MakeXRotation(float angle) { return AffineTransform(Matrix3::MakeXRotation(angle)); }
        static __forceinline AffineTransform MakeYRotation(float angle) { return AffineTransform(Matrix3::MakeYRotation(angle)); }
        static __forceinline AffineTransform MakeZRotation(float angle) { return AffineTransform(Matrix3::MakeZRotation(angle)); }
        static __forceinline AffineTransform MakeScale(float scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static __forceinline AffineTransform MakeScale(Vector3 scale) { return AffineTransform(Matrix3::MakeScale(scale)); }
        static __forceinline AffineTransform MakeTranslation(Vector3 translate) { return AffineTransform(translate); }

        __forceinline Vector3 operator* (Vector3 vec) const { return _basis * vec + _translation; }
        __forceinline AffineTransform operator* (const AffineTransform& mat) const {
            return AffineTransform(_basis * mat._basis, *this * mat.GetTranslation());
        }

    private:
        Matrix3 _basis;
        Vector3 _translation;
    };
}