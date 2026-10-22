//
// StretchedImage.h - Texture stretched to fill its bounds
//

#pragma once

#include "PrimitiveSupport.h"

namespace Engine
{
	class StretchedImage final : public IRenderPrimitive
	{
	public:
		explicit StretchedImage(AssetKey key) : m_key(std::move(key)) {}

		void Update(double) override {}

		void Draw(QuadRenderer& renderer, const AssetManager& assets, const RectF& bounds) const override
		{
			const auto* texture = FindResidentTexture(assets, m_key);
			if (texture == nullptr)
			{
				return;
			}
			renderer.Draw(texture->GpuDescriptor(), bounds, UvRectF{}, ColorF{});
		}

	private:
		AssetKey m_key;
	};
}
