//
// ModeTimer.h
//

#pragma once

#include <algorithm>

namespace Engine
{
	class ModeTimer
	{
	public:
		static constexpr double kMaxTimeScale = 4.0;

		double Advance(double coreDt) noexcept
		{
			const double scaledDt = coreDt * m_timeScale;
			m_elapsed += scaledDt;
			return scaledDt;
		}

		void Reset() noexcept
		{
			m_elapsed = 0.0;
		}

		void SetTimeScale(double scale) noexcept
		{
			m_timeScale = std::clamp(scale, 0.0, kMaxTimeScale);
		}

		double TimeScale() const noexcept { return m_timeScale; }
		double Elapsed() const noexcept { return m_elapsed; }

	private:
		double m_timeScale = 1.0;
		double m_elapsed = 0.0;
	};
}
