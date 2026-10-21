//
// EngineMode.h
//

#pragma once

#include <optional>

#include "../../DeviceResources.h"
#include "EngineState.h"
#include "Input.h"
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

		void Tick(double coreDt)
		{
			Update(ModeTick{ coreDt, m_timer.Advance(coreDt) });
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

		virtual LocalCommand HandleInput(const InputEvent& input) = 0;
		virtual void ExecuteCommand(LocalCommand command) = 0;
		virtual void Render(SharedRenderer&) {}

	protected:
		virtual void OnEnter() {}
		virtual void OnExit() {}
		virtual void Update(const ModeTick& tick) = 0;

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
	};
}
