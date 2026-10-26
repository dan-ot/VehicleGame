//
// OnScreenKeyboard.h - Shows the Windows touch keyboard (TabTip.exe) for controller text entry
//

#pragma once

#include "InputCommands.h"

#include <windows.h>
#include <shellapi.h>

namespace Engine
{
	class OnScreenKeyboard final
	{
	public:
		OnScreenKeyboard() = default;
		OnScreenKeyboard(const OnScreenKeyboard&) = delete;
		OnScreenKeyboard& operator=(const OnScreenKeyboard&) = delete;

		~OnScreenKeyboard()
		{
			Hide();
		}

		void Show(InputDevice lastDevice) noexcept
		{
			if (lastDevice != InputDevice::Controller || m_visible)
			{
				return;
			}

			m_visible = ShellExecuteW(nullptr, L"open", L"TabTip.exe", nullptr, nullptr, SW_SHOWNORMAL) > reinterpret_cast<HINSTANCE>(32);
		}

		void Hide() noexcept
		{
			if (!m_visible)
			{
				return;
			}

			if (HWND keyboard = FindWindowW(L"IPTip_Main_Window", nullptr))
			{
				PostMessageW(keyboard, WM_SYSCOMMAND, SC_CLOSE, 0);
			}
			m_visible = false;
		}

	private:
		bool m_visible = false;
	};
}
