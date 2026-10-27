//
// CoreMode.h
//

#pragma once

#include <chrono>

#include "../Assets/AssetManager.h"
#include "../Audio/Mixer.h"
#include "../Modes/LoadingMode.h"
#include "../Modes/MenuMode.h"
#include "../Modes/PlayMode.h"
#include "EngineMode.h"
#include "TransitionSpec.h"

namespace Engine
{
	class ModeKits
	{
	public:
		AssetKit& Loading() noexcept { return m_loading; }
		AssetKit& Menu() noexcept { return m_menu; }
		AssetKit& Play() noexcept { return m_play; }
		AssetKit& Staging() noexcept { return m_staging; }

	private:
		AssetKit m_loading{ RequesterId(EngineState::Loading) };
		AssetKit m_menu{ RequesterId(EngineState::Menu) };
		AssetKit m_play{ RequesterId(EngineState::Play) };
		AssetKit m_staging{ RequesterId(EngineState::Loading) };
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
			m_loading.BindMixer(m_mixer);
			m_menu.BindMixer(m_mixer);
			m_play.BindMixer(m_mixer);

			m_active = &m_loading;
			DeclareActiveAssets();
			m_assets.Declare(m_kits.Staging(), StagedManifestFor(EngineState::Menu));
			m_loading.BindStagedManifest(m_kits.Staging().Manifest());
			m_active->Enter();
		}

		~CoreMode()
		{
			m_mixer.Shutdown();
			m_assets.Release(m_mixer.PersistentKit());
			m_assets.Release(m_kits.Loading());
			m_assets.Release(m_kits.Menu());
			m_assets.Release(m_kits.Play());
			m_assets.Release(m_kits.Staging());
		}

		AssetManager& Assets() noexcept
		{
			return m_assets;
		}

		Mixer& GetMixer() noexcept
		{
			return m_mixer;
		}

		AssetKit& LoadingKit() noexcept
		{
			return m_kits.Loading();
		}

		// Supplies the transition the next state is entered with. The destination's manifest is staged
		// on the Staging kit so it is resident while Loading runs, and is released once the destination is active.
		void RequestTransition(EngineState next, TransitionSpec spec)
		{
			m_assets.Declare(m_kits.Staging(), spec.manifest);
			m_pendingSpec.emplace(next, std::move(spec));
			m_loading.BindStagedManifest(m_kits.Staging().Manifest());
		}

		void Tick(double coreDt, const CommandFrame& commands, const RawKeyboardFrame& keyboard)
		{
			m_assets.Pump();
			m_active->Tick(coreDt, commands, keyboard);
			ApplyRequestedTransition();
			m_mixer.Update();
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
			m_mixer.OnModeExit();
			m_state = *next;
			m_active = ModeFor(m_state);
			DeclareActiveAssets();
			if (m_state != EngineState::Loading)
			{
				m_assets.Release(m_kits.Staging());
			}
			m_active->Enter();
		}

		AssetManifest StagedManifestFor(EngineState state)
		{
			AssetManifest manifest;
			ModeFor(state)->DeclareAssets(manifest);
			return manifest;
		}

		void DeclareActiveAssets()
		{
			AssetKit& kit = KitFor(m_state);
			AssetManifest manifest;
			if (m_pendingSpec && m_pendingSpec->first == m_state)
			{
				manifest = std::move(m_pendingSpec->second.manifest);
				m_pendingSpec.reset();
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
		Mixer m_mixer{ m_assets };
		EngineHandoffSlot m_handoffs;
		LoadingMode m_loading;
		MenuMode m_menu;
		PlayMode m_play;
		IEngineMode* m_active = nullptr;
		std::optional<std::pair<EngineState, TransitionSpec>> m_pendingSpec;
		EngineState m_state = EngineState::Loading;
		double m_renderCost = 0.0;
	};
}
