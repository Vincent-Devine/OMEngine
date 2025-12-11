#pragma once

#include "OMEngine/Math/Vector.hpp"

namespace OM::Math
{
	class BoundingSphere
	{
	public:
		BoundingSphere() {}
		BoundingSphere(float x, float y, float z, float r) : _repr(x, y, z, r) {}
		BoundingSphere(const XMFLOAT4* unalignedArray) : _repr(*unalignedArray) {}
		BoundingSphere(Vector3 center, Scalar radius) { _repr = Vector4(center); _repr.SetW(radius); }
		BoundingSphere(EZeroTag) : _repr(kZero) {}
		explicit BoundingSphere(const XMVECTOR& v) : _repr(v) {}
		explicit BoundingSphere(const XMFLOAT4& f4) : _repr(f4) {}
		explicit BoundingSphere(Vector4 sphere) : _repr(sphere) {}
		explicit operator Vector4() const { return Vector4(_repr); }

		Vector3 GetCenter() const { return Vector3(_repr); }
		Scalar GetRadius() const { return _repr.GetW(); }

		BoundingSphere Union(const BoundingSphere& rhs);

	private:
		Vector4 _repr;
	};
}