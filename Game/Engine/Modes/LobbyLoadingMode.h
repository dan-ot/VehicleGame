//
// LobbyLoadingMode.h - Loading into a lobby; enters the Lobby Play mode
//

#pragma once

#include "LoadingModeBase.h"

namespace Engine
{
	class LobbyLoadingMode final : public LoadingModeBase
	{
	public:
		LobbyLoadingMode()
			: LoadingModeBase(ModeId::LobbyPlay)
		{
		}
	};
}
