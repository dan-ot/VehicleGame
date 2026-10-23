//
// MenuControl.h - Interface for value-bearing menu items (edit boxes, sliders)
//

#pragma once

#include "../Assets/AssetManager.h"
#include "../Render/Primitives.h"
#include "../Render/QuadRenderer.h"

namespace Engine
{
	class IMenuControl
	{
	public:
		virtual ~IMenuControl() = default;

		virtual void Update(double dt) = 0;
		virtual void Render(QuadRenderer& renderer, const AssetManager& assets, const RectF& rect) const = 0;
		virtual void Activate() = 0;
	};
}
