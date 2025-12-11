#pragma once

#include "OMEngine/Math/Scalar.hpp"
#include "OMEngine/Math/Vector.hpp"
#include "OMEngine/Math/Matrix4.hpp"
#include "OMEngine/Math/Transform.hpp"

namespace OM::Math
{
	__forceinline bool operator<  (Scalar lhs, float rhs) { return (float)lhs < rhs; }
	__forceinline bool operator<= (Scalar lhs, float rhs) { return (float)lhs <= rhs; }
	__forceinline bool operator>  (Scalar lhs, float rhs) { return (float)lhs > rhs; }
	__forceinline bool operator>= (Scalar lhs, float rhs) { return (float)lhs >= rhs; }
	__forceinline bool operator== (Scalar lhs, float rhs) { return (float)lhs == rhs; }
	__forceinline bool operator<  (float lhs, Scalar rhs) { return lhs < (float)rhs; }
	__forceinline bool operator<= (float lhs, Scalar rhs) { return lhs <= (float)rhs; }
	__forceinline bool operator>  (float lhs, Scalar rhs) { return lhs > (float)rhs; }
	__forceinline bool operator>= (float lhs, Scalar rhs) { return lhs >= (float)rhs; }
	__forceinline bool operator== (float lhs, Scalar rhs) { return lhs == (float)rhs; }

#define CREATE_SIMD_FUNCTIONS( TYPE ) \
	__forceinline TYPE Sqrt( TYPE s ) { return TYPE(XMVectorSqrt(s)); } \
	__forceinline TYPE Recip( TYPE s ) { return TYPE(XMVectorReciprocal(s)); } \
	__forceinline TYPE RecipSqrt( TYPE s ) { return TYPE(XMVectorReciprocalSqrt(s)); } \
	__forceinline TYPE Floor( TYPE s ) { return TYPE(XMVectorFloor(s)); } \
	__forceinline TYPE Ceiling( TYPE s ) { return TYPE(XMVectorCeiling(s)); } \
	__forceinline TYPE Round( TYPE s ) { return TYPE(XMVectorRound(s)); } \
	__forceinline TYPE Abs( TYPE s ) { return TYPE(XMVectorAbs(s)); } \
	__forceinline TYPE Exp( TYPE s ) { return TYPE(XMVectorExp(s)); } \
	__forceinline TYPE Pow( TYPE b, TYPE e ) { return TYPE(XMVectorPow(b, e)); } \
	__forceinline TYPE Log( TYPE s ) { return TYPE(XMVectorLog(s)); } \
	__forceinline TYPE Sin( TYPE s ) { return TYPE(XMVectorSin(s)); } \
	__forceinline TYPE Cos( TYPE s ) { return TYPE(XMVectorCos(s)); } \
	__forceinline TYPE Tan( TYPE s ) { return TYPE(XMVectorTan(s)); } \
	__forceinline TYPE ASin( TYPE s ) { return TYPE(XMVectorASin(s)); } \
	__forceinline TYPE ACos( TYPE s ) { return TYPE(XMVectorACos(s)); } \
	__forceinline TYPE ATan( TYPE s ) { return TYPE(XMVectorATan(s)); } \
	__forceinline TYPE ATan2( TYPE y, TYPE x ) { return TYPE(XMVectorATan2(y, x)); } \
	__forceinline TYPE Lerp( TYPE a, TYPE b, TYPE t ) { return TYPE(XMVectorLerpV(a, b, t)); } \
    __forceinline TYPE Lerp( TYPE a, TYPE b, float t ) { return TYPE(XMVectorLerp(a, b, t)); } \
	__forceinline TYPE Max( TYPE a, TYPE b ) { return TYPE(XMVectorMax(a, b)); } \
	__forceinline TYPE Min( TYPE a, TYPE b ) { return TYPE(XMVectorMin(a, b)); } \
	__forceinline TYPE Clamp( TYPE v, TYPE a, TYPE b ) { return Min(Max(v, a), b); } \
	__forceinline BoolVector operator<  ( TYPE lhs, TYPE rhs ) { return XMVectorLess(lhs, rhs); } \
	__forceinline BoolVector operator<= ( TYPE lhs, TYPE rhs ) { return XMVectorLessOrEqual(lhs, rhs); } \
	__forceinline BoolVector operator>  ( TYPE lhs, TYPE rhs ) { return XMVectorGreater(lhs, rhs); } \
	__forceinline BoolVector operator>= ( TYPE lhs, TYPE rhs ) { return XMVectorGreaterOrEqual(lhs, rhs); } \
	__forceinline BoolVector operator== ( TYPE lhs, TYPE rhs ) { return XMVectorEqual(lhs, rhs); } \
	__forceinline TYPE Select( TYPE lhs, TYPE rhs, BoolVector mask ) { return TYPE(XMVectorSelect(lhs, rhs, mask)); }

	CREATE_SIMD_FUNCTIONS(Scalar)
	CREATE_SIMD_FUNCTIONS(Vector3)
	CREATE_SIMD_FUNCTIONS(Vector4)

#undef CREATE_SIMD_FUNCTIONS

	__forceinline float Sqrt(float s) { return Sqrt(Scalar(s)); }
	__forceinline float Recip(float s) { return Recip(Scalar(s)); }
	__forceinline float RecipSqrt(float s) { return RecipSqrt(Scalar(s)); }
	__forceinline float Floor(float s) { return Floor(Scalar(s)); }
	__forceinline float Ceiling(float s) { return Ceiling(Scalar(s)); }
	__forceinline float Round(float s) { return Round(Scalar(s)); }
	__forceinline float Abs(float s) { return s < 0.0f ? -s : s; }
	__forceinline float Exp(float s) { return Exp(Scalar(s)); }
	__forceinline float Pow(float b, float e) { return Pow(Scalar(b), Scalar(e)); }
	__forceinline float Log(float s) { return Log(Scalar(s)); }
	__forceinline float Sin(float s) { return Sin(Scalar(s)); }
	__forceinline float Cos(float s) { return Cos(Scalar(s)); }
	__forceinline float Tan(float s) { return Tan(Scalar(s)); }
	__forceinline float ASin(float s) { return ASin(Scalar(s)); }
	__forceinline float ACos(float s) { return ACos(Scalar(s)); }
	__forceinline float ATan(float s) { return ATan(Scalar(s)); }
	__forceinline float ATan2(float y, float x) { return ATan2(Scalar(y), Scalar(x)); }
	__forceinline float Lerp(float a, float b, float t) { return a + (b - a) * t; }
	__forceinline float Max(float a, float b) { return a > b ? a : b; }
	__forceinline float Min(float a, float b) { return a < b ? a : b; }
	__forceinline float Clamp(float v, float a, float b) { return Min(Max(v, a), b); }

	__forceinline Scalar Length(Vector3 v) { return Scalar(XMVector3Length(v)); }
	__forceinline Scalar LengthSquare(Vector3 v) { return Scalar(XMVector3LengthSq(v)); }
	__forceinline Scalar LengthRecip(Vector3 v) { return Scalar(XMVector3ReciprocalLength(v)); }
	__forceinline Scalar Dot(Vector3 v1, Vector3 v2) { return Scalar(XMVector3Dot(v1, v2)); }
	__forceinline Scalar Dot(Vector4 v1, Vector4 v2) { return Scalar(XMVector4Dot(v1, v2)); }
	__forceinline Vector3 Cross(Vector3 v1, Vector3 v2) { return Vector3(XMVector3Cross(v1, v2)); }
	__forceinline Vector3 Normalize(Vector3 v) { return Vector3(XMVector3Normalize(v)); }
	__forceinline Vector4 Normalize(Vector4 v) { return Vector4(XMVector4Normalize(v)); }

	__forceinline Matrix3 Transpose(const Matrix3& mat) { return Matrix3(XMMatrixTranspose(mat)); }
	__forceinline Matrix3 InverseTranspose(const Matrix3& mat)
	{
		const Vector3 x = mat.GetX();
		const Vector3 y = mat.GetY();
		const Vector3 z = mat.GetZ();

		const Vector3 inv0 = Cross(y, z);
		const Vector3 inv1 = Cross(z, x);
		const Vector3 inv2 = Cross(x, y);
		const Scalar  rDet = Recip(Dot(z, inv2));

		// Return the adjoint / determinant
		return Matrix3(inv0, inv1, inv2) * rDet;
	}

	// inline Matrix3 Inverse( const Matrix3& mat ) { TBD }
	// inline Transform Inverse( const Transform& mat ) { TBD }

	// This specialized matrix invert assumes that the 3x3 matrix is orthogonal (and normalized).
	__forceinline AffineTransform OrthoInvert(const AffineTransform& xform)
	{
		Matrix3 basis = Transpose(xform.GetBasis());
		return AffineTransform(basis, basis * -xform.GetTranslation());
	}

	__forceinline OrthogonalTransform Invert(const OrthogonalTransform& xform) { return ~xform; }

	__forceinline Matrix4 Transpose(const Matrix4& mat) { return Matrix4(XMMatrixTranspose(mat)); }
	__forceinline Matrix4 Invert(const Matrix4& mat) { return Matrix4(XMMatrixInverse(nullptr, mat)); }

	__forceinline Matrix4 OrthoInvert(const Matrix4& xform)
	{
		Matrix3 basis = Transpose(xform.Get3x3());
		Vector3 translate = basis * -Vector3(xform.GetW());
		return Matrix4(basis, translate);
	}
}