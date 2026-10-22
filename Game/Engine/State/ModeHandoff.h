//
// ModeHandoff.h
//

#pragma once

#include <optional>
#include <string>
#include <variant>

#include "EngineState.h"

namespace Engine
{
	struct MenuHandoff
	{
		int selectedItem = 0;
	};

	struct PlayHandoff
	{
		std::wstring levelName;
	};

	struct LoadingHandoff
	{
		EngineState destination = EngineState::Menu;
	};

	using HandoffPayload = std::variant<std::monostate, MenuHandoff, PlayHandoff, LoadingHandoff>;

	struct ModeHandoff
	{
		EngineState target = EngineState::Menu;
		HandoffPayload payload;
	};

	class EngineHandoffSlot
	{
	public:
		void Publish(ModeHandoff handoff) noexcept { m_handoff = std::move(handoff); }

		std::optional<ModeHandoff> Take() noexcept
		{
			auto handoff = m_handoff;
			m_handoff.reset();
			return handoff;
		}

		bool HasPending() const noexcept { return m_handoff.has_value(); }

	private:
		std::optional<ModeHandoff> m_handoff;
	};
}
