//
// CoreMode.h
//

#pragma once

#include <chrono>
#include <memory>
#include <optional>

#include "../Assets/AssetManager.h"
#include "../Audio/Mixer.h"
#include "../Modes/LobbyLoadingMode.h"
#include "../Modes/LobbyPlayMode.h"
#include "../Modes/MainMenuMode.h"
#include "../Modes/MenuLoadingMode.h"
#include "../Modes/ModeDescriptors.h"
#include "../Modes/SplashScreenMode.h"
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
			m_active = CreateMode(ModeId::SplashScreen);
			DeclareActiveAssets();
			RequestTransition(ModeId::MainMenu);
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
		void RequestTransition(ModeId next)
		{
			TransitionSpec spec{ next, ModeDescriptors::ManifestFor(next) };
			m_assets.Declare(m_kits.Staging(), spec.manifest);
			m_pendingSpec.emplace(next, std::move(spec));
			if (ToEngineState(m_state) == EngineState::Loading)
			{
				BindStagedManifest(*m_active);
			}
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

		ModeId State() const noexcept
		{
			return m_state;
		}

	private:
		static constexpr double kRenderCostSmoothing = 0.1;

		static bool IsTransitionAllowed(ModeId from, ModeId to) noexcept
		{
			switch (from)
			{
			case ModeId::SplashScreen: return to == ModeId::MainMenu;
			case ModeId::MainMenu:     return to == ModeId::MenuLoading || to == ModeId::LobbyLoading;
			case ModeId::MenuLoading:  return to == ModeId::MainMenu || to == ModeId::LobbyLoading;
			case ModeId::LobbyLoading: return to == ModeId::LobbyPlay || to == ModeId::MainMenu;
			case ModeId::LobbyPlay:    return to == ModeId::MenuLoading;
			}
			return false;
		}

		std::unique_ptr<IEngineMode> CreateMode(ModeId mode)
		{
			std::unique_ptr<IEngineMode> created;
			AssetKit* kit = nullptr;
			switch (mode)
			{
			case ModeId::SplashScreen: created = std::make_unique<SplashScreenMode>(); kit = &m_kits.Loading(); break;
			case ModeId::MenuLoading:  created = std::make_unique<MenuLoadingMode>(); kit = &m_kits.Loading(); break;
			case ModeId::LobbyLoading: created = std::make_unique<LobbyLoadingMode>(); kit = &m_kits.Loading(); break;
			case ModeId::MainMenu:     created = std::make_unique<MainMenuMode>(); kit = &m_kits.Menu(); break;
			case ModeId::LobbyPlay:    created = std::make_unique<LobbyPlayMode>(); kit = &m_kits.Play(); break;
			}
			created->BindKit(*kit);
			created->BindHandoffs(m_handoffs);
			created->BindMixer(m_mixer);
			BindAssetsTo(*created);
			return created;
		}

		void BindAssetsTo(IEngineMode& mode)
		{
			if (auto* loading = dynamic_cast<LoadingModeBase*>(&mode))
			{
				loading->BindAssets(m_assets);
			}
			else if (auto* menu = dynamic_cast<MenuBase*>(&mode))
			{
				menu->BindAssets(m_assets);
			}
		}

		void BindStagedManifest(IEngineMode& mode)
		{
			if (auto* loading = dynamic_cast<LoadingModeBase*>(&mode))
			{
				loading->BindStagedManifest(m_kits.Staging().Manifest());
			}
		}

		void ApplyRequestedTransition()
		{
			const auto next = m_active->TakeRequestedTransition();
			if (next && IsTransitionAllowed(m_state, *next))
			{
				m_pendingDestination = next;
			}
			if (!m_pendingDestination || !AssetsReadyFor(*m_pendingDestination))
			{
				return;
			}

			const ModeId destination = *m_pendingDestination;
			m_pendingDestination.reset();
			m_active->Exit();
			m_mixer.OnModeExit();
			m_active.reset();
			m_state = destination;
			m_active = CreateMode(m_state);
			DeclareActiveAssets();
			BindStagedManifest(*m_active);
			if (ToEngineState(m_state) != EngineState::Loading)
			{
				m_assets.Release(m_kits.Staging());
			}
			m_active->Enter();
		}

		bool AssetsReadyFor(ModeId destination) const
		{
			if (!m_pendingSpec || m_pendingSpec->first != destination)
			{
				return true;
			}
			for (const AssetKey& key : m_pendingSpec->second.manifest.Keys())
			{
				if (!m_assets.IsBuilt(key) && !m_assets.HasFailed(key))
				{
					return false;
				}
			}
			return true;
		}

		void DeclareActiveAssets()
		{
			AssetKit& kit = KitFor(ToEngineState(m_state));
			AssetManifest manifest;
			if (m_pendingSpec && m_pendingSpec->first == m_state)
			{
				manifest = std::move(m_pendingSpec->second.manifest);
				m_pendingSpec.reset();
			}
			else
			{
				manifest = ModeDescriptors::ManifestFor(m_state);
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
		std::unique_ptr<IEngineMode> m_active;
		std::optional<ModeId> m_pendingDestination;
		std::optional<std::pair<ModeId, TransitionSpec>> m_pendingSpec;
		ModeId m_state = ModeId::SplashScreen;
		double m_renderCost = 0.0;
	};
}
