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

		void Draw(Renderer& renderer, const RectF& bounds) const override
		{
			renderer.Draw(m_key, bounds, UvRectF{}, ColorF{});
		}

	private:
		AssetKey m_key;
	};
}
