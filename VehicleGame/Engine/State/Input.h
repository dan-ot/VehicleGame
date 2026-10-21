//
// Input.h
//

#pragma once

namespace Engine
{
	enum class InputSource
	{
		Pointer,
		Gamepad,
		Keyboard
	};

	struct InputEvent
	{
		InputSource source;
		int code;
		float x;
		float y;
		bool pressed;
	};

	enum class LocalCommand
	{
		None,
		Up,
		Down,
		Select,
		Back,
		Accelerate,
		Brake
	};
}
