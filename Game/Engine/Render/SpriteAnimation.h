//
// SpriteAnimation.h - Frame-based animation over a grid sprite sheet
//

#pragma once

#include "PrimitiveSupport.h"

#include <cmath>

namespace Engine
{
	class SpriteAnimation final : public IRenderPrimitive
	{
	public:
		SpriteAnimation(AssetKey key, uint32_t columns, uint32_t rows, uint32_t frameCount, double frameDuration)
			: m_key(std::move(key)), m_columns(columns), m_rows(rows), m_frameCount(frameCount), m_frameDuration(frameDuration) {}

		void Update(double dt) override { Advance(dt); }

		void Advance(double dt)
		{
			const double cycle = m_frameDuration * m_frameCount;
			m_elapsed += dt;
			if (cycle > 0.0 && m_elapsed >= cycle)
			{
				m_elapsed = std::fmod(m_elapsed, cycle);
			}
		}

		uint32_t CurrentFrame() const noexcept
		{
			if (m_frameDuration <= 0.0 || m_frameCount == 0)
			{
				return 0;
			}
			const auto frame = static_cast<uint32_t>(m_elapsed / m_frameDuration);
			return std::min(frame, m_frameCount - 1);
		}

		UvRectF CurrentUv() const noexcept
		{
			const uint32_t frame = CurrentFrame();
			const uint32_t column = frame % m_columns;
			const uint32_t row = frame / m_columns;
			const float cellWidth = 1.0f / static_cast<float>(m_columns);
			const float cellHeight = 1.0f / static_cast<float>(m_rows);
			return UvRectF{
				column * cellWidth,
				row * cellHeight,
				(column + 1) * cellWidth,
				(row + 1) * cellHeight };
		}

		void Draw(QuadRenderer& renderer, const AssetManager& assets, const RectF& bounds) const override
		{
			const auto* texture = FindResidentTexture(assets, m_key);
			if (texture == nullptr)
			{
				return;
			}
			renderer.Draw(texture->GpuDescriptor(), bounds, CurrentUv(), ColorF{});
		}

	private:
		AssetKey m_key;
		uint32_t m_columns;
		uint32_t m_rows;
		uint32_t m_frameCount;
		double m_frameDuration;
		double m_elapsed = 0.0;
	};
}
