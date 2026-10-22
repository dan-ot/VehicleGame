//
// PrimitiveSupport.h - Texture lookup and reveal math shared by image primitives
//

#pragma once

#include "QuadRenderer.h"
#include "../Assets/AssetManager.h"
#include "../Assets/TextureResource.h"

#include <algorithm>

namespace Engine
{
	// Returns the texture when it is declared and resident on the GPU, otherwise nullptr.
	inline const TextureResource* FindResidentTexture(const AssetManager& assets, const AssetKey& key)
	{
		const auto* texture = dynamic_cast<const TextureResource*>(assets.Find(key));
		if (texture == nullptr || texture->GpuDescriptor().ptr == 0)
		{
			return nullptr;
		}
		return texture;
	}

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
