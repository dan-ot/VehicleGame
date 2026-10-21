//
// MenuMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class MenuMode final : public IEngineMode
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
			case VK_UP:     return LocalCommand::Up;
			case VK_DOWN:   return LocalCommand::Down;
			case VK_RETURN: return LocalCommand::Select;
			case VK_ESCAPE: return LocalCommand::Back;
			default:        return LocalCommand::None;
			}
		}

		void ExecuteCommand(LocalCommand command) override
		{
			switch (command)
			{
			case LocalCommand::Up:
				if (m_selected > 0)
				{
					--m_selected;
				}
				break;

			case LocalCommand::Down:
				if (m_selected < kItemCount - 1)
				{
					++m_selected;
				}
				break;

			case LocalCommand::Select:
				RequestTransition(m_selected == 0 ? EngineState::Play : EngineState::Loading);
				break;

			default:
				break;
			}
		}

	protected:
		void Update(const ModeTick&) override
		{
		}

	private:
		static constexpr int kItemCount = 2;
		int m_selected = 0;
	};
}
