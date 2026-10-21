//
// InputDevices.h
//

#pragma once

#include <algorithm>
#include <vector>

#include <XInput.h>

#include "InputSystem.h"

#pragma comment(lib, "xinput.lib")

namespace Engine
{
	class InputDevices final
	{
	public:
		void Initialize(HWND window)
		{
			m_window = window;

			RAWINPUTDEVICE device = {};
			device.usUsagePage = 0x01;
			device.usUsage = 0x02;
			device.hwndTarget = window;
			RegisterRawInputDevices(&device, 1, sizeof(device));
		}

		void OnRawInput(LPARAM lParam)
		{
			UINT size = 0;
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, nullptr, &size, sizeof(RAWINPUTHEADER)) != 0)
			{
				return;
			}

			std::vector<BYTE> buffer(size);
			if (GetRawInputData(reinterpret_cast<HRAWINPUT>(lParam), RID_INPUT, buffer.data(), &size, sizeof(RAWINPUTHEADER)) != size)
			{
				return;
			}

			const auto& raw = *reinterpret_cast<const RAWINPUT*>(buffer.data());
			if (raw.header.dwType != RIM_TYPEMOUSE)
			{
				return;
			}

			const auto& mouse = raw.data.mouse;
			if (!(mouse.usFlags & MOUSE_MOVE_ABSOLUTE))
			{
				m_pendingDeltaX += mouse.lLastX;
				m_pendingDeltaY += mouse.lLastY;
			}

			ApplyButtonFlags(mouse.usButtonFlags, MouseButton::Left, RI_MOUSE_LEFT_BUTTON_DOWN, RI_MOUSE_LEFT_BUTTON_UP);
			ApplyButtonFlags(mouse.usButtonFlags, MouseButton::Right, RI_MOUSE_RIGHT_BUTTON_DOWN, RI_MOUSE_RIGHT_BUTTON_UP);
			ApplyButtonFlags(mouse.usButtonFlags, MouseButton::Middle, RI_MOUSE_MIDDLE_BUTTON_DOWN, RI_MOUSE_MIDDLE_BUTTON_UP);
			ApplyButtonFlags(mouse.usButtonFlags, MouseButton::X1, RI_MOUSE_BUTTON_4_DOWN, RI_MOUSE_BUTTON_4_UP);
			ApplyButtonFlags(mouse.usButtonFlags, MouseButton::X2, RI_MOUSE_BUTTON_5_DOWN, RI_MOUSE_BUTTON_5_UP);

			if (mouse.usButtonFlags & RI_MOUSE_WHEEL)
			{
				m_pendingWheel += static_cast<SHORT>(mouse.usButtonData);
			}
		}

		void OnFocusLost() noexcept
		{
			m_mouseButtons = {};
			m_pendingDeltaX = 0;
			m_pendingDeltaY = 0;
			m_pendingWheel = 0;
		}

		RawInputSample Sample() noexcept
		{
			RawInputSample sample;
			sample.gamepad = ReadGamepad();

			if (GetForegroundWindow() == m_window)
			{
				for (std::size_t i = 0; i < kKeyCount; ++i)
				{
					sample.keys[i] = (GetAsyncKeyState(static_cast<int>(i)) & 0x8000) != 0;
				}
			}

			sample.mouse.deltaX = m_pendingDeltaX;
			sample.mouse.deltaY = m_pendingDeltaY;
			sample.mouse.wheel = m_pendingWheel;
			sample.mouse.buttons = m_mouseButtons;

			m_pendingDeltaX = 0;
			m_pendingDeltaY = 0;
			m_pendingWheel = 0;

			return sample;
		}

	private:
		static RawGamepad ReadGamepad() noexcept
		{
			static constexpr WORD kButtonMasks[kGamepadButtonCount] =
			{
				XINPUT_GAMEPAD_A,
				XINPUT_GAMEPAD_B,
				XINPUT_GAMEPAD_X,
				XINPUT_GAMEPAD_Y,
				XINPUT_GAMEPAD_LEFT_SHOULDER,
				XINPUT_GAMEPAD_RIGHT_SHOULDER,
				XINPUT_GAMEPAD_START,
				XINPUT_GAMEPAD_BACK,
				XINPUT_GAMEPAD_LEFT_THUMB,
				XINPUT_GAMEPAD_RIGHT_THUMB,
				XINPUT_GAMEPAD_DPAD_UP,
				XINPUT_GAMEPAD_DPAD_DOWN,
				XINPUT_GAMEPAD_DPAD_LEFT,
				XINPUT_GAMEPAD_DPAD_RIGHT
			};

			RawGamepad pad;
			for (DWORD user = 0; user < XUSER_MAX_COUNT; ++user)
			{
				XINPUT_STATE state = {};
				if (XInputGetState(user, &state) != ERROR_SUCCESS)
				{
					continue;
				}

				const auto& gamepad = state.Gamepad;
				pad.connected = true;
				for (std::size_t i = 0; i < kGamepadButtonCount; ++i)
				{
					pad.buttons[i] = (gamepad.wButtons & kButtonMasks[i]) != 0;
				}
				pad.leftX = Thumb(gamepad.sThumbLX);
				pad.leftY = Thumb(gamepad.sThumbLY);
				pad.rightX = Thumb(gamepad.sThumbRX);
				pad.rightY = Thumb(gamepad.sThumbRY);
				pad.leftTrigger = static_cast<float>(gamepad.bLeftTrigger) / 255.0f;
				pad.rightTrigger = static_cast<float>(gamepad.bRightTrigger) / 255.0f;
				break;
			}

			return pad;
		}

		static float Thumb(SHORT value) noexcept
		{
			return std::max(-1.0f, static_cast<float>(value) / 32767.0f);
		}

		void ApplyButtonFlags(USHORT flags, MouseButton button, USHORT downFlag, USHORT upFlag) noexcept
		{
			auto& down = m_mouseButtons[static_cast<std::size_t>(button)];
			if (flags & downFlag)
			{
				down = true;
			}
			else if (flags & upFlag)
			{
				down = false;
			}
		}

		HWND m_window = nullptr;
		std::array<bool, kMouseButtonCount> m_mouseButtons{};
		LONG m_pendingDeltaX = 0;
		LONG m_pendingDeltaY = 0;
		LONG m_pendingWheel = 0;
	};
}
