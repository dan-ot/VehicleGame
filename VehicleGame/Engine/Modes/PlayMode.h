//
// PlayMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class PlayMode final : public IEngineMode
	{
	public:
		LocalCommand HandleInput(const InputEvent& input) override
		{
			if (input.source != InputSource::Keyboard || !input.pressed)
			{
				return LocalCommand::None;
			}

			switch (input.code)
			{
			case 'W':       return LocalCommand::Accelerate;
			case 'S':       return LocalCommand::Brake;
			case VK_ESCAPE: return LocalCommand::Back;
			default:        return LocalCommand::None;
			}
		}

		void ExecuteCommand(LocalCommand command) override
		{
			switch (command)
			{
			case LocalCommand::Back:
				RequestTransition(EngineState::Menu);
				break;

			default:
				m_lastCommand = command;
				break;
			}
		}

	protected:
		void Update(const ModeTick&) override
		{
		}

	private:
		LocalCommand m_lastCommand = LocalCommand::None;
	};
}
