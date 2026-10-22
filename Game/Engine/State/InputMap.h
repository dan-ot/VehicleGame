//
// InputMap.h
//

#pragma once

#include <array>
#include <cstddef>
#include <cstdint>

#include "InputCommands.h"
#include "InputFrame.h"

namespace Engine
{
	enum class InputAxis : std::uint16_t
	{
		LeftX,
		LeftY,
		RightX,
		RightY,
		LeftTrigger,
		RightTrigger,
		MouseX,
		MouseY
	};

	struct KeyBinding
	{
		std::uint16_t key;
		Command command;
	};

	struct GamepadBinding
	{
		GamepadButton button;
		Command command;
	};

	struct MouseButtonBinding
	{
		MouseButton button;
		Command command;
	};

	struct AxisBinding
	{
		InputDevice device;
		InputAxis axis;
		bool negative;
		Command command;
	};

	inline constexpr std::array kKeyBindings{
		KeyBinding{ 'W', Command::Movement_Up },
		KeyBinding{ 'S', Command::Movement_Down },
		KeyBinding{ 'A', Command::Movement_Left },
		KeyBinding{ 'D', Command::Movement_Right },
		KeyBinding{ VK_UP, Command::Direction_Up },
		KeyBinding{ VK_DOWN, Command::Direction_Down },
		KeyBinding{ VK_LEFT, Command::Direction_Left },
		KeyBinding{ VK_RIGHT, Command::Direction_Right },
		KeyBinding{ 'Z', Command::Trigger_Left2 },
		KeyBinding{ 'C', Command::Trigger_Right2 },
		KeyBinding{ VK_LCONTROL, Command::Modifier_Left },
		KeyBinding{ VK_LSHIFT, Command::Modifier_Right },
		KeyBinding{ 'F', Command::Action_A },
		KeyBinding{ 'E', Command::Action_B },
		KeyBinding{ 'Q', Command::Action_X },
		KeyBinding{ 'R', Command::Action_Y },
		KeyBinding{ VK_RETURN, Command::System_Accept },
		KeyBinding{ VK_ESCAPE, Command::System_Abort }
	};

	inline constexpr std::array kGamepadBindings{
		GamepadBinding{ GamepadButton::DpadUp, Command::Direction_Up },
		GamepadBinding{ GamepadButton::DpadDown, Command::Direction_Down },
		GamepadBinding{ GamepadButton::DpadLeft, Command::Direction_Left },
		GamepadBinding{ GamepadButton::DpadRight, Command::Direction_Right },
		GamepadBinding{ GamepadButton::LeftShoulder, Command::Trigger_Left2 },
		GamepadBinding{ GamepadButton::RightShoulder, Command::Trigger_Right2 },
		GamepadBinding{ GamepadButton::LeftThumb, Command::Modifier_Left },
		GamepadBinding{ GamepadButton::RightThumb, Command::Modifier_Right },
		GamepadBinding{ GamepadButton::A, Command::Action_A },
		GamepadBinding{ GamepadButton::B, Command::Action_B },
		GamepadBinding{ GamepadButton::X, Command::Action_X },
		GamepadBinding{ GamepadButton::Y, Command::Action_Y },
		GamepadBinding{ GamepadButton::Start, Command::System_Accept },
		GamepadBinding{ GamepadButton::Back, Command::System_Abort }
	};

	inline constexpr std::array kMouseButtonBindings{
		MouseButtonBinding{ MouseButton::Left, Command::Trigger_Left },
		MouseButtonBinding{ MouseButton::Right, Command::Trigger_Right },
		MouseButtonBinding{ MouseButton::Middle, Command::Trigger_Left2 },
		MouseButtonBinding{ MouseButton::X1, Command::Trigger_Right2 }
	};

	inline constexpr std::array kAxisBindings{
		AxisBinding{ InputDevice::Controller, InputAxis::LeftY, false, Command::Movement_Up },
		AxisBinding{ InputDevice::Controller, InputAxis::LeftY, true, Command::Movement_Down },
		AxisBinding{ InputDevice::Controller, InputAxis::LeftX, true, Command::Movement_Left },
		AxisBinding{ InputDevice::Controller, InputAxis::LeftX, false, Command::Movement_Right },
		AxisBinding{ InputDevice::Controller, InputAxis::RightY, false, Command::Navigate_Up },
		AxisBinding{ InputDevice::Controller, InputAxis::RightY, true, Command::Navigate_Down },
		AxisBinding{ InputDevice::Controller, InputAxis::RightX, true, Command::Navigate_Left },
		AxisBinding{ InputDevice::Controller, InputAxis::RightX, false, Command::Navigate_Right },
		AxisBinding{ InputDevice::Controller, InputAxis::LeftTrigger, false, Command::Trigger_Left },
		AxisBinding{ InputDevice::Controller, InputAxis::RightTrigger, false, Command::Trigger_Right },
		AxisBinding{ InputDevice::Mouse, InputAxis::MouseY, true, Command::Navigate_Up },
		AxisBinding{ InputDevice::Mouse, InputAxis::MouseY, false, Command::Navigate_Down },
		AxisBinding{ InputDevice::Mouse, InputAxis::MouseX, true, Command::Navigate_Left },
		AxisBinding{ InputDevice::Mouse, InputAxis::MouseX, false, Command::Navigate_Right }
	};
}
