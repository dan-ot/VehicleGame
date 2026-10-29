//
// ModeId.h - Concrete mode identity; each mode maps onto one generic EngineState
//

#pragma once

#include "EngineState.h"

namespace Engine
{
	enum class ModeId
	{
		SplashScreen,
		MainMenu,
		MenuLoading,
		LobbyLoading,
		LobbyPlay,
	};

	inline EngineState ToEngineState(ModeId mode) noexcept
	{
		switch (mode)
		{
		case ModeId::SplashScreen:
		case ModeId::MenuLoading:
		case ModeId::LobbyLoading: return EngineState::Loading;
		case ModeId::MainMenu:     return EngineState::Menu;
		case ModeId::LobbyPlay:    return EngineState::Play;
		}
		return EngineState::Loading;
	}
}
