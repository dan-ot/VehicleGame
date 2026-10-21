//
// InputFrame.h
//

#pragma once

#include <array>
#include <cstddef>

namespace Engine
{
	constexpr std::size_t kKeyCount = 256;

	enum class GamepadButton : std::size_t
	{
		A,
		B,
		X,
		Y,
		LeftShoulder,
		RightShoulder,
		Start,
		Back,
		LeftThumb,
		RightThumb,
		DpadUp,
		DpadDown,
		DpadLeft,
		DpadRight,
		Count
	};

	enum class MouseButton : std::size_t
	{
		Left,
		Right,
		Middle,
		X1,
		X2,
		Count
	};

	constexpr std::size_t kGamepadButtonCount = static_cast<std::size_t>(GamepadButton::Count);
	constexpr std::size_t kMouseButtonCount = static_cast<std::size_t>(MouseButton::Count);

	struct ButtonState
	{
		bool down = false;
		bool pressed = false;
		bool released = false;

		void Advance(bool isDown) noexcept
		{
			pressed = isDown && !down;
			released = !isDown && down;
			down = isDown;
		}
	};

	struct GamepadState
	{
		bool connected = false;
		std::array<ButtonState, kGamepadButtonCount> buttons{};
		float leftX = 0.0f;
		float leftY = 0.0f;
		float rightX = 0.0f;
		float rightY = 0.0f;
		float leftTrigger = 0.0f;
		float rightTrigger = 0.0f;
	};

	struct KeyboardState
	{
		std::array<ButtonState, kKeyCount> keys{};
	};

	struct MouseState
	{
		float x = 0.0f;
		float y = 0.0f;
		float wheel = 0.0f;
		std::array<ButtonState, kMouseButtonCount> buttons{};
	};

	struct InputFrame
	{
		GamepadState gamepad;
		KeyboardState keyboard;
		MouseState mouse;
	};
}
