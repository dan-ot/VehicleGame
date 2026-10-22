//
// Scene.h - Minimal empty scene placeholder owned by PlayMode
//

#pragma once

namespace Engine
{
	class Scene final
	{
	public:
		void Update(double) noexcept {}
		bool IsEmpty() const noexcept { return true; }
	};
}
