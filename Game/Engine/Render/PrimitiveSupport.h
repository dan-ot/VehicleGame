//
// PrimitiveSupport.h - Reveal math shared by image primitives
//

#pragma once

#include "QuadRenderer.h"

#include <algorithm>

namespace Engine
{
	// Clips the destination and UV range from the left edge to the given visible fraction.
	inline void RevealLeft(const RectF& bounds, const UvRectF& uv, float percent,
		RectF& destination, UvRectF& revealedUv)
	{
		const float fraction = std::clamp(percent, 0.0f, 1.0f);

		destination = bounds;
		destination.width = bounds.width * fraction;

		revealedUv = uv;
		revealedUv.u1 = uv.u0 + (uv.u1 - uv.u0) * fraction;
	}
}
