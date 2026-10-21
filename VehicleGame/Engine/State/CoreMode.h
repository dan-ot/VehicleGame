//
// CoreMode.h
//

#pragma once

#include <chrono>

#include "../Modes/LoadingMode.h"
#include "../Modes/MenuMode.h"
#include "../Modes/PlayMode.h"
#include "EngineMode.h"

namespace Engine
{
	class CoreMode final
	{
	public:
		CoreMode()
		{
			m_active = &m_loading;
			m_active->Enter();
		}

		void Tick(double coreDt)
		{
			m_active->Tick(coreDt);
			ApplyRequestedTransition();
		}

		void HandleInput(const InputEvent& input)
		{
			m_active->ExecuteCommand(m_active->HandleInput(input));
			ApplyRequestedTransition();
		}

		void Render(SharedRenderer& renderer)
		{
			const auto start = std::chrono::steady_clock::now();
			m_active->Render(renderer);
			const double cost = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();

			m_renderCost = m_renderCost == 0.0 ? cost : m_renderCost + kRenderCostSmoothing * (cost - m_renderCost);
		}

		double RenderCostEstimate() const noexcept
		{
			return m_renderCost;
		}

		EngineState State() const noexcept
		{
			return m_state;
		}

	private:
		static constexpr double kRenderCostSmoothing = 0.1;

		static bool IsTransitionAllowed(EngineState from, EngineState to) noexcept
		{
			switch (from)
			{
			case EngineState::Loading: return to == EngineState::Menu;
			case EngineState::Menu:    return to == EngineState::Play || to == EngineState::Loading;
			case EngineState::Play:    return to == EngineState::Menu;
			}
			return false;
		}

		IEngineMode* ModeFor(EngineState state) noexcept
		{
			switch (state)
			{
			case EngineState::Loading: return &m_loading;
			case EngineState::Menu:    return &m_menu;
			case EngineState::Play:    return &m_play;
			}
			return &m_loading;
		}

		void ApplyRequestedTransition()
		{
			const auto next = m_active->TakeRequestedTransition();
			if (!next || !IsTransitionAllowed(m_state, *next))
			{
				return;
			}

			m_active->Exit();
			m_state = *next;
			m_active = ModeFor(m_state);
			m_active->Enter();
		}

		LoadingMode m_loading;
		MenuMode m_menu;
		PlayMode m_play;
		IEngineMode* m_active = nullptr;
		EngineState m_state = EngineState::Loading;
		double m_renderCost = 0.0;
	};
}
