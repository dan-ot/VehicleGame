//
// InputSystem.h
//

#pragma once

#include <algorithm>
#include <cmath>

#include "InputCommands.h"
#include "InputFrame.h"
#include "InputMap.h"
#include "RawKeyboardFrame.h"

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
		std::u32string characters;
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
				if (raw.keys[i] && !m_keyboard.keyDown[i])
				{
					m_keyboard.keyPressed[i] = true;
					m_lastDevice = InputDevice::Keyboard;
				}
				m_keyboard.keyDown[i] = raw.keys[i];
			}
			m_keyboard.shift = raw.keys[kVkShift];
			m_keyboard.ctrl = raw.keys[kVkControl];
			m_keyboard.characters += raw.characters;

			m_frame.mouse.deltaX = static_cast<float>(raw.mouse.deltaX);
			m_frame.mouse.deltaY = static_cast<float>(raw.mouse.deltaY);
			m_frame.mouse.wheel = static_cast<float>(raw.mouse.wheel);
			for (std::size_t i = 0; i < kMouseButtonCount; ++i)
			{
				m_frame.mouse.buttons[i].Advance(raw.mouse.buttons[i]);
			}

			if (!m_keyboard.characters.empty())
			{
				m_lastDevice = InputDevice::Keyboard;
			}
			if (raw.mouse.deltaX != 0 || raw.mouse.deltaY != 0 || raw.mouse.wheel != 0 || AnyMouseButton(raw.mouse))
			{
				m_lastDevice = InputDevice::Mouse;
			}
			if (AnyGamepadActivity(raw.gamepad))
			{
				m_lastDevice = InputDevice::Controller;
			}

			BuildCommands(raw);

			return m_frame;
		}

		const CommandFrame& Commands() const noexcept
		{
			return m_commands;
		}

		void ResetDevices() noexcept
		{
			m_frame.keyboard = {};
			m_frame.mouse.buttons = {};
			m_keyboard = {};
		}

		const InputFrame& Frame() const noexcept
		{
			return m_frame;
		}

		InputDevice LastDevice() const noexcept
		{
			return m_lastDevice;
		}

		RawKeyboardFrame TakeKeyboard() noexcept
		{
			RawKeyboardFrame latched = m_keyboard;
			m_keyboard.characters.clear();
			m_keyboard.keyPressed = {};
			return latched;
		}

	private:
		static constexpr float kStickDeadzone = 0.24f;
		static constexpr float kTriggerDeadzone = 0.12f;
		static constexpr std::size_t kVkShift = 0x10;
		static constexpr std::size_t kVkControl = 0x11;

		static bool AnyMouseButton(const RawMouse& mouse) noexcept
		{
			return std::any_of(mouse.buttons.begin(), mouse.buttons.end(), [](bool down) { return down; });
		}

		static bool AnyGamepadActivity(const RawGamepad& gamepad) noexcept
		{
			const bool buttonDown = std::any_of(gamepad.buttons.begin(), gamepad.buttons.end(), [](bool down) { return down; });
			return buttonDown || std::fabs(gamepad.leftX) > kStickDeadzone || std::fabs(gamepad.leftY) > kStickDeadzone
				|| std::fabs(gamepad.rightX) > kStickDeadzone || std::fabs(gamepad.rightY) > kStickDeadzone
				|| gamepad.leftTrigger > kTriggerDeadzone || gamepad.rightTrigger > kTriggerDeadzone;
		}

		static bool IsNavigation(Command command) noexcept
		{
			return command >= Command::Navigate_Up && command <= Command::Navigate_Right;
		}

		void Contribute(Command command, InputDevice device, std::uint16_t component, float strength) noexcept
		{
			auto& value = m_commands.values[static_cast<std::size_t>(command)];
			auto& slot = value.stack[static_cast<std::size_t>(device)];
			if (!slot || strength > slot->strength)
			{
				slot = Contribution{ device, component, strength };
			}

			if (device == InputDevice::Mouse && IsNavigation(command))
			{
				return;
			}

			if (!value.greatest.valid || strength > value.greatest.strength)
			{
				value.greatest = CommandValue::Greatest{ strength, device, true };
			}
		}

		void BuildCommands(const RawInputSample& raw) noexcept
		{
			m_commands = {};

			for (const auto& binding : kKeyBindings)
			{
				if (raw.keys[binding.key])
				{
					Contribute(binding.command, InputDevice::Keyboard, binding.key, 1.0f);
				}
			}

			for (const auto& binding : kGamepadBindings)
			{
				if (m_frame.gamepad.buttons[static_cast<std::size_t>(binding.button)].down)
				{
					Contribute(binding.command, InputDevice::Controller, static_cast<std::uint16_t>(binding.button), 1.0f);
				}
			}

			for (const auto& binding : kMouseButtonBindings)
			{
				if (m_frame.mouse.buttons[static_cast<std::size_t>(binding.button)].down)
				{
					Contribute(binding.command, InputDevice::Mouse, static_cast<std::uint16_t>(binding.button), 1.0f);
				}
			}

			for (const auto& binding : kAxisBindings)
			{
				const float value = AxisValue(binding.axis);
				const float strength = binding.negative ? -value : value;
				if (strength > 0.0f)
				{
					Contribute(binding.command, binding.device, static_cast<std::uint16_t>(binding.axis), strength);
				}
			}
		}

		float AxisValue(InputAxis axis) const noexcept
		{
			const auto& gamepad = m_frame.gamepad;
			switch (axis)
			{
			case InputAxis::LeftX:        return gamepad.leftX;
			case InputAxis::LeftY:        return gamepad.leftY;
			case InputAxis::RightX:       return gamepad.rightX;
			case InputAxis::RightY:       return gamepad.rightY;
			case InputAxis::LeftTrigger:  return gamepad.leftTrigger;
			case InputAxis::RightTrigger: return gamepad.rightTrigger;
			case InputAxis::MouseX:       return m_frame.mouse.deltaX;
			case InputAxis::MouseY:       return m_frame.mouse.deltaY;
			}
			return 0.0f;
		}

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
		CommandFrame m_commands;
		RawKeyboardFrame m_keyboard;
		InputDevice m_lastDevice = InputDevice::Keyboard;
	};
}
