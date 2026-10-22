//
// AssetRequester.h - Identifies which retained asset kit holds a reference to an asset
//

#pragma once

#include "../State/EngineState.h"

namespace Engine
{
	class RequesterId
	{
	public:
		constexpr explicit RequesterId(EngineState state) noexcept : m_state(state) {}

		constexpr EngineState State() const noexcept { return m_state; }

		constexpr bool operator==(const RequesterId& other) const noexcept { return m_state == other.m_state; }
		constexpr bool operator!=(const RequesterId& other) const noexcept { return m_state != other.m_state; }

	private:
		EngineState m_state;
	};
}
