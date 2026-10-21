//
// LoadingMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class LoadingMode final : public IEngineMode
	{
	protected:
		void Update(const ModeTick&, const CommandFrame&) override
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
