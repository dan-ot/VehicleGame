//
// SplashScreenMode.h - Startup splash; stages the Main Menu
//

#pragma once

#include "LoadingModeBase.h"

namespace Engine
{
	class SplashScreenMode final : public LoadingModeBase
	{
	public:
		SplashScreenMode()
			: LoadingModeBase(ModeId::MainMenu)
		{
		}
	};
}
