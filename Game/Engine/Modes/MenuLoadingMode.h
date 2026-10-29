//
// MenuLoadingMode.h - Loading between menu screens; returns to the Main Menu
//

#pragma once

#include "LoadingModeBase.h"

namespace Engine
{
	class MenuLoadingMode final : public LoadingModeBase
	{
	public:
		MenuLoadingMode()
			: LoadingModeBase(ModeId::MainMenu)
		{
		}
	};
}
