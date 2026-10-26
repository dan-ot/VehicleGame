//
// RawKeyboardFrame.h
//

#pragma once

#include <array>
#include <string>

#include "InputFrame.h"

namespace Engine
{
	struct RawKeyboardFrame
	{
		std::u32string characters;
		std::array<bool, kKeyCount> keyDown{};
		std::array<bool, kKeyCount> keyPressed{};
		bool shift = false;
		bool ctrl = false;
		InputDevice lastDevice = InputDevice::Keyboard;
	};
}
