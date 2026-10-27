//
// TransitionSpec.h - Destination state and the assets it needs before it can be entered
//

#pragma once

#include "../Assets/AssetManifest.h"
#include "EngineState.h"

namespace Engine
{
	// Sound and music decode on the CPU path; everything else needs a GPU build.
	inline bool UsesCpuPath(AssetType type) noexcept
	{
		return type == AssetType::Sound || type == AssetType::Music;
	}

	struct TransitionSpec
	{
		EngineState destination = EngineState::Menu;
		AssetManifest manifest;
	};
}
