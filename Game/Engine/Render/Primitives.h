//
// Primitives.h - Shared 2D rendering primitive contract
//

#pragma once

#include "RenderTypes.h"

namespace Engine
{
	class Renderer;

	class IRenderPrimitive
	{
	public:
		virtual ~IRenderPrimitive() = default;

		virtual void Update(double dt) = 0;
		virtual void Draw(Renderer& renderer, const RectF& bounds) const = 0;
	};
}
