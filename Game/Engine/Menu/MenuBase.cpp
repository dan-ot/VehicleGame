//
// MenuBase.cpp
//

#include "pch.h"
#include "MenuBase.h"

#include <cassert>

namespace Engine
{
	MenuBase::MenuBase(AssetKey cursorKey)
		: m_cursor(std::move(cursorKey), kCursorSize)
	{
		m_cursor.SetScaling(CursorScaling{ kStickSpeedPixels, kMousePixelsPerUnit });
	}

	void MenuBase::OnEnter()
	{
		if (m_root == nullptr)
		{
			m_root = std::make_unique<MenuLevel>();
			BuildRoot(*m_root);
			assert(m_root->Background() != nullptr && "Menu root level requires a background");
		}
	}

	void MenuBase::Render(SharedRenderer& renderer)
	{
		const auto viewport = renderer.GetScreenViewport();
		m_screenWidth = viewport.Width;
		m_screenHeight = viewport.Height;
		if (m_quads == nullptr || m_device != renderer.GetD3DDevice())
		{
			m_quads = std::make_unique<QuadRenderer>(renderer);
			m_device = renderer.GetD3DDevice();
		}

		if (m_assets == nullptr || m_root == nullptr)
		{
			return;
		}

		m_root->Recalculate(m_screenWidth, m_screenHeight);
		m_cursor.EnsurePlaced(m_screenWidth, m_screenHeight);
		m_root->Render(*m_quads, *m_assets, RectF{ 0.0f, 0.0f, viewport.Width, viewport.Height });
		OnRender(*m_quads, *m_assets);
		m_cursor.Render(*m_quads, *m_assets);
	}

	MenuInputResult MenuBase::ConsumeInput(const CommandFrame& commands, double dt)
	{
		assert(m_root != nullptr);

		MenuInputResult result;

		const CursorPosition before = m_cursor.Position();
		const CursorPosition cursor = m_cursor.ProcessInput(commands, dt, m_screenWidth, m_screenHeight);
		m_cursor.Update(dt);
		if (cursor.x != before.x || cursor.y != before.y)
		{
			const auto hovered = m_root->ItemAt(cursor.x + kCursorSize * 0.5f, cursor.y + kCursorSize * 0.5f);
			if (hovered && m_root->Item(*hovered).CanFocus() && m_root->Focus() != hovered)
			{
				FocusItem(*hovered, result);
			}
		}

		const auto direction = ActiveDirection(commands);
		if (!direction)
		{
			m_repeatTimer = 0.0;
			m_held.reset();
		}
		else if (!m_held || *m_held != *direction)
		{
			m_held = direction;
			m_repeatTimer = 0.0;
			Step(*direction, result);
		}
		else
		{
			m_repeatTimer += dt;
			if (m_repeatTimer >= kRepeatInterval)
			{
				m_repeatTimer -= kRepeatInterval;
				Step(*direction, result);
			}
		}

		const bool accept = IsActive(commands, Command::System_Accept);
		const bool abort = IsActive(commands, Command::System_Abort);
		result.accept = accept && !m_wasAccept;
		result.abort = abort && !m_wasAbort;
		m_wasAccept = accept;
		m_wasAbort = abort;

		m_root->RouteInput(ResidualCommands(commands), result.valueChanges);

		result.cursor = m_cursor.Position();
		return result;
	}

	void MenuBase::UpdateLevel(double dt)
	{
		if (m_root != nullptr)
		{
			m_root->Update(dt);
		}
	}

	CommandFrame MenuBase::ResidualCommands(const CommandFrame& commands) noexcept
	{
		CommandFrame residual = commands;
		for (std::size_t i = 0; i < kCommandCount; ++i)
		{
			const auto command = static_cast<Command>(i);
			const bool consumed = command == Command::System_Accept
				|| command == Command::System_Abort
				|| command == Command::Navigate_Up || command == Command::Navigate_Down
				|| command == Command::Navigate_Left || command == Command::Navigate_Right
				|| command == Command::Direction_Up || command == Command::Direction_Down
				|| command == Command::Direction_Left || command == Command::Direction_Right
				|| command == Command::Movement_Up || command == Command::Movement_Down
				|| command == Command::Movement_Left || command == Command::Movement_Right;
			if (consumed)
			{
				residual.values[i] = CommandValue{};
			}
		}
		return residual;
	}

	bool MenuBase::IsActive(const CommandFrame& commands, Command command) noexcept
	{
		return commands[command].greatest.valid && commands[command].greatest.strength >= 0.5f;
	}

	std::optional<MenuDirection> MenuBase::ActiveDirection(const CommandFrame& commands) noexcept
	{
		if (IsActive(commands, Command::Direction_Up) || IsActive(commands, Command::Movement_Up))
		{
			return MenuDirection::Up;
		}
		if (IsActive(commands, Command::Direction_Down) || IsActive(commands, Command::Movement_Down))
		{
			return MenuDirection::Down;
		}
		if (IsActive(commands, Command::Direction_Left) || IsActive(commands, Command::Movement_Left))
		{
			return MenuDirection::Left;
		}
		if (IsActive(commands, Command::Direction_Right) || IsActive(commands, Command::Movement_Right))
		{
			return MenuDirection::Right;
		}
		return std::nullopt;
	}

	void MenuBase::Step(MenuDirection direction, MenuInputResult& result)
	{
		const auto focus = m_root->Focus();
		if (!focus)
		{
			return;
		}

		const auto next = m_root->Next(*focus, direction);
		if (!next || !m_root->Item(*next).CanFocus())
		{
			return;
		}

		FocusItem(*next, result);
		CenterCursorOn(*next);
	}

	void MenuBase::FocusItem(MenuItemId id, MenuInputResult& result)
	{
		m_root->SetFocus(id);
		result.focusChanged = id;
	}

	void MenuBase::CenterCursorOn(MenuItemId id)
	{
		const RectF rect = m_root->ItemRect(id);
		m_cursor.PlaceAt(rect.x + rect.width * 0.5f - kCursorSize * 0.5f,
			rect.y + rect.height * 0.5f - kCursorSize * 0.5f);
	}
}
