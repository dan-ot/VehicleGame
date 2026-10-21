//
// PlayMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class PlayMode final : public IEngineMode
	{
	protected:
		void Update(const ModeTick&, const CommandFrame& commands) override
		{
			if (commands[Command::System_Abort].greatest.valid)
			{
				RequestTransition(EngineState::Menu);
				return;
			}

			m_throttle = Strength(commands[Command::Movement_Up]);
			m_brake = Strength(commands[Command::Movement_Down]);
		}

	private:
		static float Strength(const CommandValue& value) noexcept
		{
			return value.greatest.valid ? value.greatest.strength : 0.0f;
		}

		float m_throttle = 0.0f;
		float m_brake = 0.0f;
	};
}
