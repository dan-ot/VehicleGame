//
// CoreMode.h
//

#pragma once

#include <chrono>

#include "../Assets/AssetManager.h"
#include "../Modes/LoadingMode.h"
#include "../Modes/MenuMode.h"
#include "../Modes/PlayMode.h"
#include "EngineMode.h"

namespace Engine
{
	class ModeKits
	{
	public:
		AssetKit& Loading() noexcept { return m_loading; }
		AssetKit& Menu() noexcept { return m_menu; }
		AssetKit& Play() noexcept { return m_play; }

	private:
		AssetKit m_loading{ RequesterId(EngineState::Loading) };
		AssetKit m_menu{ RequesterId(EngineState::Menu) };
		AssetKit m_play{ RequesterId(EngineState::Play) };
	};

	class CoreMode final
	{
	public:
		CoreMode()
		{
			m_loading.BindKit(m_kits.Loading());
			m_menu.BindKit(m_kits.Menu());
			m_play.BindKit(m_kits.Play());
			m_loading.BindAssets(m_assets);
			m_menu.BindAssets(m_assets);
			m_loading.BindHandoffs(m_handoffs);
			m_menu.BindHandoffs(m_handoffs);
			m_play.BindHandoffs(m_handoffs);

			m_active = &m_loading;
			DeclareActiveAssets();
			m_active->Enter();
		}

		~CoreMode()
		{
			m_assets.Release(m_kits.Loading());
			m_assets.Release(m_kits.Menu());
			m_assets.Release(m_kits.Play());
		}

		AssetManager& Assets() noexcept
		{
			return m_assets;
		}

		AssetKit& LoadingKit() noexcept
		{
			return m_kits.Loading();
		}

		// Supplies the manifest the target state declares when it is entered. Modes that do not
		// call this fall back to their own DeclareAssets override.
		void RequestTransition(EngineState next, AssetManifest manifest)
		{
			m_pendingManifest.emplace(next, std::move(manifest));
		}

		void Tick(double coreDt, const CommandFrame& commands, const RawKeyboardFrame& keyboard)
		{
			m_active->Tick(coreDt, commands, keyboard);
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
			case EngineState::Loading: return to == EngineState::Menu || to == EngineState::Play;
			case EngineState::Menu:    return to == EngineState::Play || to == EngineState::Loading;
			case EngineState::Play:    return to == EngineState::Menu || to == EngineState::Loading;
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
			DeclareActiveAssets();
			m_active->Enter();
		}

		void DeclareActiveAssets()
		{
			AssetKit& kit = KitFor(m_state);
			AssetManifest manifest;
			if (m_pendingManifest && m_pendingManifest->first == m_state)
			{
				manifest = std::move(m_pendingManifest->second);
				m_pendingManifest.reset();
			}
			else
			{
				m_active->DeclareAssets(manifest);
			}
			m_assets.Declare(kit, manifest);
		}

		AssetKit& KitFor(EngineState state) noexcept
		{
			switch (state)
			{
			case EngineState::Loading: return m_kits.Loading();
			case EngineState::Menu:    return m_kits.Menu();
			case EngineState::Play:    return m_kits.Play();
			}
			return m_kits.Loading();
		}

		ModeKits m_kits;
		AssetManager m_assets;
		EngineHandoffSlot m_handoffs;
		LoadingMode m_loading;
		MenuMode m_menu;
		PlayMode m_play;
		IEngineMode* m_active = nullptr;
		std::optional<std::pair<EngineState, AssetManifest>> m_pendingManifest;
		EngineState m_state = EngineState::Loading;
		double m_renderCost = 0.0;
	};
}
