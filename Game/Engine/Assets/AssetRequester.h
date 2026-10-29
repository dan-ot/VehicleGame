//
// AssetRequester.h - Identifies which retained asset kit holds a reference to an asset
//

#pragma once

#include "../State/EngineState.h"

#include <cstddef>
#include <functional>

namespace Engine
{
	enum class PersistentOwner : unsigned char
	{
		None,
		Mixer,
		Renderer
	};

	class RequesterId
	{
	public:
		constexpr explicit RequesterId(EngineState state) noexcept : m_state(state), m_persistent(PersistentOwner::None) {}
		constexpr explicit RequesterId(PersistentOwner owner) noexcept : m_state(EngineState::Loading), m_persistent(owner) {}

		constexpr bool IsPersistent() const noexcept { return m_persistent != PersistentOwner::None; }
		constexpr PersistentOwner Persistent() const noexcept { return m_persistent; }
		constexpr EngineState State() const noexcept { return m_state; }

		constexpr bool operator==(const RequesterId& other) const noexcept
		{
			return m_persistent == other.m_persistent && m_state == other.m_state;
		}
		constexpr bool operator!=(const RequesterId& other) const noexcept { return !(*this == other); }

		constexpr std::size_t Hash() const noexcept
		{
			return (static_cast<std::size_t>(m_persistent) << 8) ^ static_cast<std::size_t>(m_state);
		}

	private:
		EngineState m_state;
		PersistentOwner m_persistent;
	};
}

template <>
struct std::hash<Engine::RequesterId>
{
	std::size_t operator()(const Engine::RequesterId& id) const noexcept { return id.Hash(); }
};
