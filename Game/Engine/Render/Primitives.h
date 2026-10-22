//
// Primitives.h - Shared 2D rendering primitive contract
//

#pragma once

namespace Engine
{
	class QuadRenderer;
	class AssetManager;

	struct RectF
	{
		float x = 0.0f;
		float y = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
	};

	class IRenderPrimitive
	{
	public:
		virtual ~IRenderPrimitive() = default;

		virtual void Update(double dt) = 0;
		virtual void Draw(QuadRenderer& renderer, const AssetManager& assets, const RectF& bounds) const = 0;
	};
}
