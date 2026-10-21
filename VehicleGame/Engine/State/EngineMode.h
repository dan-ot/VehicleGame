//
// EngineMode.h
//

#pragma once

#include <optional>

#include "../../DeviceResources.h"
#include "../Assets/AssetKit.h"
#include "../Assets/AssetManifest.h"
#include "EngineState.h"
#include "InputCommands.h"
#include "ModeTimer.h"

namespace Engine
{
	using SharedRenderer = DX::DeviceResources;

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

		void Tick(double coreDt, const CommandFrame& commands)
		{
			Update(ModeTick{ coreDt, m_timer.Advance(coreDt) }, commands);
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

	protected:
		virtual void OnEnter() {}
		virtual void OnExit() {}
		virtual void Update(const ModeTick& tick, const CommandFrame& commands) = 0;

		void RequestTransition(EngineState next) noexcept
		{
			m_requested = next;
		}

		double Elapsed() const noexcept
		{
			return m_timer.Elapsed();
		}

	private:
		ModeTimer m_timer;
		std::optional<EngineState> m_requested;
		AssetKit* m_kit = nullptr;
	};
}
