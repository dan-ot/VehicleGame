//
// SoftwareCursor.h - Screen-space cursor driven by navigation input
//

#pragma once

#include "SpriteAnimation.h"
#include "../State/InputCommands.h"

#include <algorithm>

namespace Engine
{
	struct CursorScaling
	{
		float stickPixelsPerSecond = 900.0f;
		float mousePixelsPerUnit = 1.0f;
	};

	struct CursorPosition
	{
		float x = 0.0f;
		float y = 0.0f;
	};

	class SoftwareCursor final
	{
	public:
		SoftwareCursor(AssetKey key, float size, uint32_t columns = 1, uint32_t rows = 1, uint32_t frameCount = 1, double frameDuration = 0.0)
			: m_sprite(std::move(key), columns, rows, frameCount, frameDuration)
			, m_size(size)
		{
		}

		void SetScaling(CursorScaling scaling) noexcept { m_scaling = scaling; }
		void SetSize(float size) noexcept { m_size = size; }
		CursorPosition Position() const noexcept { return m_position; }

		void PlaceAt(float x, float y) noexcept
		{
			m_position = CursorPosition{ x, y };
			m_placed = true;
		}

		void EnsurePlaced(float screenWidth, float screenHeight) noexcept
		{
			if (!m_placed)
			{
				PlaceAt(screenWidth * 0.5f, screenHeight * 0.5f);
			}
		}

		// Controller strength is normalized and scaled by time; mouse strength is a raw per-frame delta.
		CursorPosition ProcessInput(const CommandFrame& commands, double dt, float screenWidth, float screenHeight) noexcept
		{
			const float stepSeconds = static_cast<float>(dt);
			const float stickX = Axis(commands, Command::Navigate_Left, Command::Navigate_Right, InputDevice::Controller);
			const float stickY = Axis(commands, Command::Navigate_Up, Command::Navigate_Down, InputDevice::Controller);
			const float mouseX = Axis(commands, Command::Navigate_Left, Command::Navigate_Right, InputDevice::Mouse);
			const float mouseY = Axis(commands, Command::Navigate_Up, Command::Navigate_Down, InputDevice::Mouse);

			m_position.x += stickX * m_scaling.stickPixelsPerSecond * stepSeconds + mouseX * m_scaling.mousePixelsPerUnit;
			m_position.y += stickY * m_scaling.stickPixelsPerSecond * stepSeconds + mouseY * m_scaling.mousePixelsPerUnit;

			const float maxX = std::max(0.0f, screenWidth - m_size);
			const float maxY = std::max(0.0f, screenHeight - m_size);
			m_position.x = std::clamp(m_position.x, 0.0f, maxX);
			m_position.y = std::clamp(m_position.y, 0.0f, maxY);
			return m_position;
		}

		void Update(double dt) noexcept
		{
			m_sprite.Advance(dt);
		}

		void Render(QuadRenderer& renderer, const AssetManager& assets) const
		{
			m_sprite.Draw(renderer, assets, RectF{ m_position.x, m_position.y, m_size, m_size });
		}

	private:
		static float StackStrength(const CommandValue& value, InputDevice device) noexcept
		{
			const auto& contribution = value.stack[static_cast<std::size_t>(device)];
			return contribution ? contribution->strength : 0.0f;
		}

		static float Axis(const CommandFrame& commands, Command negative, Command positive, InputDevice device) noexcept
		{
			return StackStrength(commands[positive], device) - StackStrength(commands[negative], device);
		}

		SpriteAnimation m_sprite;
		float m_size;
		CursorScaling m_scaling;
		CursorPosition m_position;
		bool m_placed = false;
	};
}
