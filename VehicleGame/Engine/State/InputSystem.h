//
// InputSystem.h
//

#pragma once

#include <cmath>

#include "InputFrame.h"

namespace Engine
{
	struct RawGamepad
	{
		bool connected = false;
		std::array<bool, kGamepadButtonCount> buttons{};
		float leftX = 0.0f;
		float leftY = 0.0f;
		float rightX = 0.0f;
		float rightY = 0.0f;
		float leftTrigger = 0.0f;
		float rightTrigger = 0.0f;
	};

	struct RawMouse
	{
		int deltaX = 0;
		int deltaY = 0;
		int wheel = 0;
		std::array<bool, kMouseButtonCount> buttons{};
	};

	struct RawInputSample
	{
		RawGamepad gamepad;
		std::array<bool, kKeyCount> keys{};
		RawMouse mouse;
	};

	class InputSystem final
	{
	public:
		const InputFrame& Advance(const RawInputSample& raw) noexcept
		{
			auto& gamepad = m_frame.gamepad;
			gamepad.connected = raw.gamepad.connected;
			for (std::size_t i = 0; i < kGamepadButtonCount; ++i)
			{
				gamepad.buttons[i].Advance(raw.gamepad.buttons[i]);
			}
			gamepad.leftX = ApplyDeadzone(raw.gamepad.leftX, kStickDeadzone);
			gamepad.leftY = ApplyDeadzone(raw.gamepad.leftY, kStickDeadzone);
			gamepad.rightX = ApplyDeadzone(raw.gamepad.rightX, kStickDeadzone);
			gamepad.rightY = ApplyDeadzone(raw.gamepad.rightY, kStickDeadzone);
			gamepad.leftTrigger = ApplyDeadzone(raw.gamepad.leftTrigger, kTriggerDeadzone);
			gamepad.rightTrigger = ApplyDeadzone(raw.gamepad.rightTrigger, kTriggerDeadzone);

			for (std::size_t i = 0; i < kKeyCount; ++i)
			{
				m_frame.keyboard.keys[i].Advance(raw.keys[i]);
			}

			m_frame.mouse.x += static_cast<float>(raw.mouse.deltaX);
			m_frame.mouse.y += static_cast<float>(raw.mouse.deltaY);
			m_frame.mouse.wheel += static_cast<float>(raw.mouse.wheel);
			for (std::size_t i = 0; i < kMouseButtonCount; ++i)
			{
				m_frame.mouse.buttons[i].Advance(raw.mouse.buttons[i]);
			}

			return m_frame;
		}

		void ResetDevices() noexcept
		{
			m_frame.keyboard = {};
			m_frame.mouse.buttons = {};
		}

		const InputFrame& Frame() const noexcept
		{
			return m_frame;
		}

	private:
		static constexpr float kStickDeadzone = 0.24f;
		static constexpr float kTriggerDeadzone = 0.12f;

		static float ApplyDeadzone(float value, float deadzone) noexcept
		{
			const float magnitude = std::fabs(value);
			if (magnitude <= deadzone)
			{
				return 0.0f;
			}

			const float sign = value < 0.0f ? -1.0f : 1.0f;
			return sign * (magnitude - deadzone) / (1.0f - deadzone);
		}

		InputFrame m_frame;
	};
}
