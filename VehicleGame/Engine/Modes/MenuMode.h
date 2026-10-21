//
// MenuMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class MenuMode final : public IEngineMode
	{
	protected:
		void Update(const ModeTick& tick, const CommandFrame& commands) override
		{
			const int direction = Direction(commands);
			if (direction == 0)
			{
				m_repeatTimer = 0.0;
				m_held = 0;
			}
			else if (direction != m_held)
			{
				m_held = direction;
				m_repeatTimer = 0.0;
				Step(direction);
			}
			else
			{
				m_repeatTimer += tick.modeDt;
				if (m_repeatTimer >= kRepeatInterval)
				{
					m_repeatTimer -= kRepeatInterval;
					Step(direction);
				}
			}

			if (IsActive(commands, Command::System_Accept) && !m_wasAccept)
			{
				RequestTransition(m_selected == 0 ? EngineState::Play : EngineState::Loading);
			}
			m_wasAccept = IsActive(commands, Command::System_Accept);
		}

	private:
		static bool IsActive(const CommandFrame& commands, Command command) noexcept
		{
			return commands[command].greatest.valid && commands[command].greatest.strength >= 0.5f;
		}

		static int Direction(const CommandFrame& commands) noexcept
		{
			if (IsActive(commands, Command::Direction_Up) || IsActive(commands, Command::Movement_Up))
			{
				return -1;
			}
			if (IsActive(commands, Command::Direction_Down) || IsActive(commands, Command::Movement_Down))
			{
				return 1;
			}
			return 0;
		}

		void Step(int direction) noexcept
		{
			const int next = m_selected + direction;
			if (next >= 0 && next < kItemCount)
			{
				m_selected = next;
			}
		}

		static constexpr int kItemCount = 2;
		static constexpr double kRepeatInterval = 0.12;
		int m_selected = 0;
		int m_held = 0;
		double m_repeatTimer = 0.0;
		bool m_wasAccept = false;
	};
}
