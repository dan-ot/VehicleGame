//
// RevealedImage.h - Texture revealed left to right by a visible percentage
//

#pragma once

#include "PrimitiveSupport.h"

namespace Engine
{
	class RevealedImage final : public IRenderPrimitive
	{
	public:
		explicit RevealedImage(AssetKey key) : m_key(std::move(key)) {}

		void SetPercent(float percent) noexcept { m_percent = std::clamp(percent, 0.0f, 1.0f); }
		float Percent() const noexcept { return m_percent; }

		void Update(double) override {}

		void Draw(QuadRenderer& renderer, const AssetManager& assets, const RectF& bounds) const override
		{
			const auto* texture = FindResidentTexture(assets, m_key);
			if (texture == nullptr || m_percent <= 0.0f)
			{
				return;
			}
			RectF destination;
			UvRectF uv;
			RevealLeft(bounds, UvRectF{}, m_percent, destination, uv);
			renderer.Draw(texture->GpuDescriptor(), destination, uv, ColorF{});
		}

	private:
		AssetKey m_key;
		float m_percent = 0.0f;
	};
}
