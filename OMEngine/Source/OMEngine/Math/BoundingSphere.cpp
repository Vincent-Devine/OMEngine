#include "pch.h"
#include "OMEngine/Math/BoundingSphere.hpp"
#include "OMEngine/Math/Functions.hpp"

namespace OM::Math
{
    BoundingSphere BoundingSphere::Union(const BoundingSphere& rhs)
    {
        float radA = GetRadius();
        if (radA == 0.0f)
            return rhs;

        float radB = rhs.GetRadius();
        if (radB == 0.0f)
            return *this;

        Vector3 diff = GetCenter() - rhs.GetCenter();
        float dist = OM::Math::Length(diff);

        // Safe normalize vector between sphere centers
        diff = dist < 1e-6f ? Vector3(kXUnitVector) : diff * OM::Math::Recip(dist);

        Vector3 extremeA = GetCenter() + diff * OM::Math::Max(radA, radB - dist);
        Vector3 extremeB = rhs.GetCenter() - diff * OM::Math::Max(radB, radA - dist);

        return BoundingSphere((extremeA + extremeB) * 0.5f, OM::Math::Length(extremeA - extremeB) * 0.5f);
    }
}