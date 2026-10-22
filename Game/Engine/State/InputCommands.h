//
// InputCommands.h
//

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>
#include <optional>

namespace Engine
{
	enum class InputDevice : std::size_t
	{
		Mouse,
		Keyboard,
		Controller,
		Count
	};

	constexpr std::size_t kInputDeviceCount = static_cast<std::size_t>(InputDevice::Count);

	enum class Command : std::size_t
	{
		Movement_Up,
		Movement_Down,
		Movement_Left,
		Movement_Right,
		Navigate_Up,
		Navigate_Down,
		Navigate_Left,
		Navigate_Right,
		Direction_Up,
		Direction_Down,
		Direction_Left,
		Direction_Right,
		Trigger_Left,
		Trigger_Right,
		Trigger_Left2,
		Trigger_Right2,
		Modifier_Left,
		Modifier_Right,
		Action_A,
		Action_B,
		Action_X,
		Action_Y,
		System_Accept,
		System_Abort,
		Count
	};

	constexpr std::size_t kCommandCount = static_cast<std::size_t>(Command::Count);

	struct Contribution
	{
		InputDevice device = InputDevice::Mouse;
		std::uint16_t component = 0;
		float strength = 0.0f;
	};

	struct CommandValue
	{
		struct Greatest
		{
			float strength = 0.0f;
			InputDevice device = InputDevice::Mouse;
			bool valid = false;
		};

		std::array<std::optional<Contribution>, kInputDeviceCount> stack{};
		Greatest greatest{};
	};

	struct CommandFrame
	{
		std::array<CommandValue, kCommandCount> values{};

		const CommandValue& operator[](Command command) const noexcept
		{
			return values[static_cast<std::size_t>(command)];
		}
	};
}
