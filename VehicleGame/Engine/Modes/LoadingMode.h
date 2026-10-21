//
// LoadingMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class LoadingMode final : public IEngineMode
	{
	public:
		LocalCommand HandleInput(const InputEvent&) override
		{
			return LocalCommand::None;
		}

		void ExecuteCommand(LocalCommand) override
		{
		}

	protected:
		void Update(const ModeTick&) override
		{
			if (Elapsed() >= kLoadSeconds)
			{
				RequestTransition(EngineState::Menu);
			}
		}

	private:
		static constexpr double kLoadSeconds = 2.0;
	};
}
