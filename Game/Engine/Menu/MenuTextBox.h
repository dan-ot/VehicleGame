//
// MenuTextBox.h - Single-line text entry control (free text, numeric, password)
//

#pragma once

#include "MenuControl.h"
#include "../Render/TextBlock.h"

#include <array>
#include <cstddef>
#include <string>

namespace Engine
{
	enum class MenuTextMode
	{
		FreeText,
		Numeric,
		Password,
	};

	struct MenuTextAssets
	{
		AssetKey fontKey;
		const IFontData* font = nullptr;
		AssetKey whiteKey;
		bool whiteResident = false;
	};

	class MenuTextBox final : public MenuControl<std::wstring>
	{
	public:
		static constexpr std::size_t kMaxLength = 32;

		explicit MenuTextBox(MenuTextMode mode = MenuTextMode::FreeText, std::wstring initial = {})
			: MenuControl<std::wstring>(std::move(initial))
			, m_mode(mode)
			, m_caret(Value().size())
		{
		}

		MenuTextMode Mode() const noexcept { return m_mode; }
		std::size_t Caret() const noexcept { return m_caret; }

		std::wstring DisplayText() const
		{
			if (m_mode == MenuTextMode::Password)
			{
				return std::wstring(Value().size(), L'*');
			}
			return Value();
		}

		void Update(double dt) override { m_lastDt = dt; }

		void SetAssets(MenuTextAssets assets) noexcept { m_assets = assets; }

		void Render(Renderer& renderer, const RectF& rect) const override
		{
			if (!m_assets.whiteResident)
			{
				return;
			}

			const ColorF border{ 1.0f, 1.0f, 1.0f, 1.0f };
			const float thickness = 2.0f;
			renderer.Draw(m_assets.whiteKey, RectF{ rect.x, rect.y, rect.width, thickness }, UvRectF{}, border);
			renderer.Draw(m_assets.whiteKey, RectF{ rect.x, rect.y + rect.height - thickness, rect.width, thickness }, UvRectF{}, border);
			renderer.Draw(m_assets.whiteKey, RectF{ rect.x, rect.y, thickness, rect.height }, UvRectF{}, border);
			renderer.Draw(m_assets.whiteKey, RectF{ rect.x + rect.width - thickness, rect.y, thickness, rect.height }, UvRectF{}, border);

			if (m_assets.font == nullptr)
			{
				return;
			}

			const float textX = rect.x + kPadding;
			const float textY = rect.y + kPadding;
			m_text.SetText(ToUtf8(DisplayText()), m_assets.fontKey, *m_assets.font, m_style, textX, textY);
			m_text.Draw(renderer);

			if (m_focused)
			{
				const float caretX = textX + static_cast<float>(m_caret) * kCaretAdvance;
				renderer.Draw(m_assets.whiteKey, RectF{ caretX, textY, thickness, rect.height - 2.0f * kPadding }, UvRectF{}, border);
			}
		}

		void SetFocused(bool focused) noexcept { m_focused = focused; }

	protected:
		MenuInputOutcome OnInput(const CommandFrame&, const RawKeyboardFrame& keyboard, std::wstring& value) override
		{
			if (m_caret > value.size())
			{
				m_caret = value.size();
			}

			bool handled = false;
			const bool ctrl = keyboard.ctrl;

			const auto repeating = [&](std::size_t vk) {
				const bool pressed = keyboard.keyPressed[vk];
				const bool down = keyboard.keyDown[vk];
				auto& timer = m_repeat[vk];
				if (pressed)
				{
					timer = kRepeatDelay;
					return true;
				}
				if (!down)
				{
					timer = 0.0;
					return false;
				}
				timer -= m_lastDt;
				if (timer <= 0.0)
				{
					timer += kRepeatInterval;
					return true;
				}
				return false;
			};

			if (repeating(kVkBack))
			{
				handled = true;
				const std::size_t start = ctrl ? WordLeft(value) : (m_caret > 0 ? m_caret - 1 : 0);
				EraseRange(value, start, m_caret);
			}
			else if (repeating(kVkDelete))
			{
				handled = true;
				const std::size_t end = ctrl ? WordRight(value) : (m_caret < value.size() ? m_caret + 1 : value.size());
				EraseRange(value, m_caret, end);
			}
			else if (repeating(kVkLeft))
			{
				handled = true;
				m_caret = ctrl ? WordLeft(value) : (m_caret > 0 ? m_caret - 1 : 0);
			}
			else if (repeating(kVkRight))
			{
				handled = true;
				m_caret = ctrl ? WordRight(value) : (m_caret < value.size() ? m_caret + 1 : value.size());
			}
			else if (keyboard.keyPressed[kVkHome])
			{
				handled = true;
				m_caret = 0;
			}
			else if (keyboard.keyPressed[kVkEnd])
			{
				handled = true;
				m_caret = value.size();
			}

			for (const char32_t codepoint : keyboard.characters)
			{
				if (!Accepts(codepoint, value))
				{
					continue;
				}
				handled = true;
				value.insert(value.begin() + static_cast<std::ptrdiff_t>(m_caret), static_cast<wchar_t>(codepoint));
				++m_caret;
			}

			return handled ? MenuInputOutcome::Handled : MenuInputOutcome::Ignored;
		}

	private:
		static constexpr std::size_t kVkBack = 0x08;
		static constexpr std::size_t kVkHome = 0x24;
		static constexpr std::size_t kVkEnd = 0x23;
		static constexpr std::size_t kVkLeft = 0x25;
		static constexpr std::size_t kVkRight = 0x27;
		static constexpr std::size_t kVkDelete = 0x2E;

		static bool IsSpace(wchar_t c) noexcept
		{
			return c == L' ';
		}

		std::size_t WordLeft(const std::wstring& value) const noexcept
		{
			std::size_t pos = m_caret;
			while (pos > 0 && IsSpace(value[pos - 1])) --pos;
			while (pos > 0 && !IsSpace(value[pos - 1])) --pos;
			return pos;
		}

		std::size_t WordRight(const std::wstring& value) const noexcept
		{
			std::size_t pos = m_caret;
			while (pos < value.size() && !IsSpace(value[pos])) ++pos;
			while (pos < value.size() && IsSpace(value[pos])) ++pos;
			return pos;
		}

		void EraseRange(std::wstring& value, std::size_t start, std::size_t end)
		{
			if (start >= end)
			{
				return;
			}
			value.erase(start, end - start);
			m_caret = start;
		}

		bool Accepts(char32_t codepoint, const std::wstring& value) const noexcept
		{
			if (value.size() >= kMaxLength || codepoint < 0x20 || codepoint > 0xFFFF || (codepoint >= 0xD800 && codepoint <= 0xDFFF))
			{
				return false;
			}

			if (m_mode != MenuTextMode::Numeric)
			{
				return true;
			}

			if (codepoint >= L'0' && codepoint <= L'9')
			{
				return true;
			}
			return codepoint == L'-' && m_caret == 0 && value.find(L'-') == std::wstring::npos;
		}

		static constexpr double kRepeatDelay = 0.4;
		static constexpr double kRepeatInterval = 0.05;
		static constexpr float kPadding = 8.0f;
		static constexpr float kCaretAdvance = 16.0f;

		static std::string ToUtf8(const std::wstring& text)
		{
			std::string out;
			out.reserve(text.size());
			for (const wchar_t c : text)
			{
				out.push_back(c < 0x80 ? static_cast<char>(c) : '?');
			}
			return out;
		}

		MenuTextMode m_mode;
		std::size_t m_caret;
		double m_lastDt = 0.0;
		std::array<double, 256> m_repeat{};
		MenuTextAssets m_assets{};
		bool m_focused = false;
		TextStyle m_style{};
		mutable TextBlock m_text;
	};
}
