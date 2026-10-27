//
// EngineMode.h
//

#pragma once

#include <optional>

#include "../Render/QuadRenderer.h"
#include "../Audio/Mixer.h"
#include "../Assets/AssetKit.h"
#include "../Assets/AssetManifest.h"
#include "EngineState.h"
#include "InputCommands.h"
#include "ModeHandoff.h"
#include "RawKeyboardFrame.h"
#include "ModeTimer.h"

namespace Engine
{
	using SharedRenderer = Renderer;
	using SharedMixer = Mixer;

	struct ModeTick
	{
		double coreDt;
		double modeDt;
	};

	class IEngineMode
	{
	public:
		virtual ~IEngineMode() = default;

		void Enter()
		{
			m_timer.Reset();
			OnEnter();
		}

		void Exit()
		{
			OnExit();
		}

		void Tick(double coreDt, const CommandFrame& commands, const RawKeyboardFrame& keyboard)
		{
			Update(ModeTick{ coreDt, m_timer.Advance(coreDt) }, commands, keyboard);
		}

		void SetTimeScale(double scale) noexcept
		{
			m_timer.SetTimeScale(scale);
		}

		std::optional<EngineState> TakeRequestedTransition() noexcept
		{
			auto requested = m_requested;
			m_requested.reset();
			return requested;
		}

		virtual void Render(SharedRenderer&) {}

		virtual void DeclareAssets(AssetManifest&) {}

		AssetKit& Assets() noexcept
		{
			return *m_kit;
		}

		void BindKit(AssetKit& kit) noexcept
		{
			m_kit = &kit;
		}

		void BindHandoffs(EngineHandoffSlot& slot) noexcept
		{
			m_handoffs = &slot;
		}

		void BindMixer(SharedMixer& mixer) noexcept
		{
			m_mixer = &mixer;
		}

	protected:
		virtual void OnEnter() {}
		virtual void OnExit() {}
		virtual void Update(const ModeTick& tick, const CommandFrame& commands, const RawKeyboardFrame& keyboard) = 0;

		void RequestTransition(EngineState next) noexcept
		{
			m_requested = next;
		}

		SharedMixer& GetMixer() noexcept
		{
			return *m_mixer;
		}

		void PublishHandoff(ModeHandoff handoff) noexcept
		{
			if (m_handoffs != nullptr)
			{
				m_handoffs->Publish(std::move(handoff));
			}
		}

		std::optional<ModeHandoff> TakeHandoff() noexcept
		{
			return m_handoffs != nullptr ? m_handoffs->Take() : std::nullopt;
		}

		double Elapsed() const noexcept
		{
			return m_timer.Elapsed();
		}

	private:
		ModeTimer m_timer;
		std::optional<EngineState> m_requested;
		AssetKit* m_kit = nullptr;
		EngineHandoffSlot* m_handoffs = nullptr;
		SharedMixer* m_mixer = nullptr;
	};
}
