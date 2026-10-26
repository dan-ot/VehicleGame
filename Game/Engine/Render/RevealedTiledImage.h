//
// RevealedTiledImage.h - Tiled texture revealed left to right by a visible percentage
//

#pragma once

#include "PrimitiveSupport.h"

namespace Engine
{
	class RevealedTiledImage final : public IRenderPrimitive
	{
	public:
		RevealedTiledImage(AssetKey key, float tileWidth, float tileHeight)
			: m_key(std::move(key)), m_tileWidth(tileWidth), m_tileHeight(tileHeight) {}

		void SetPercent(float percent) noexcept { m_percent = std::clamp(percent, 0.0f, 1.0f); }
		float Percent() const noexcept { return m_percent; }

		void Update(double) override {}

		void Draw(Renderer& renderer, const RectF& bounds) const override
		{
			if (m_percent <= 0.0f)
			{
				return;
			}
			const UvRectF tiled{ 0.0f, 0.0f, bounds.width / m_tileWidth, bounds.height / m_tileHeight };
			RectF destination;
			UvRectF uv;
			RevealLeft(bounds, tiled, m_percent, destination, uv);
			renderer.Draw(m_key, destination, uv, ColorF{});
		}

	private:
		AssetKey m_key;
		float m_tileWidth;
		float m_tileHeight;
		float m_percent = 0.0f;
	};
}
