//
// TiledImage.h - Texture repeated at a fixed tile size across its bounds
//

#pragma once

#include "PrimitiveSupport.h"

namespace Engine
{
	class TiledImage final : public IRenderPrimitive
	{
	public:
		TiledImage(AssetKey key, float tileWidth, float tileHeight)
			: m_key(std::move(key)), m_tileWidth(tileWidth), m_tileHeight(tileHeight) {}

		void Update(double) override {}

		void Draw(QuadRenderer& renderer, const AssetManager& assets, const RectF& bounds) const override
		{
			const auto* texture = FindResidentTexture(assets, m_key);
			if (texture == nullptr)
			{
				return;
			}
			const UvRectF uv{ 0.0f, 0.0f, bounds.width / m_tileWidth, bounds.height / m_tileHeight };
			renderer.Draw(texture->GpuDescriptor(), bounds, uv, ColorF{});
		}

	private:
		AssetKey m_key;
		float m_tileWidth;
		float m_tileHeight;
	};
}
