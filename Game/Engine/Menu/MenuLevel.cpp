//
// MenuLevel.cpp
//

#include "pch.h"
#include "MenuLevel.h"
#include "MenuControl.h"

#include <any>

namespace Engine
{
	MenuLevel::MenuLevel(MenuLevel* parent)
		: m_parent(parent)
	{
		m_root = m_layout.CreateNode();
		m_layout.SetFlexDirectionColumn(m_root);
		m_layout.SetJustify(m_root, LayoutAlign::Center);
		m_layout.SetAlignItems(m_root, LayoutAlign::Center);
	}

	void MenuLevel::SetBackground(AssetKey key)
	{
		m_background.emplace(std::move(key));
	}

	void MenuLevel::SetFocusGraphic(AssetKey key)
	{
		m_focusGraphic.emplace(std::move(key));
	}

	const StretchedImage* MenuLevel::Background() const noexcept
	{
		for (const MenuLevel* level = this; level != nullptr; level = level->m_parent)
		{
			if (level->m_background)
			{
				return &*level->m_background;
			}
		}
		return nullptr;
	}

	MenuItemId MenuLevel::AddItem(MenuItem item, float width, float height)
	{
		item.layoutNode = m_layout.CreateNode();
		m_layout.SetFixedSize(item.layoutNode, width, height);
		m_layout.AddChild(m_root, item.layoutNode);

		const MenuItemId id = m_items.size();
		const bool focusable = item.CanFocus();
		m_items.push_back(std::move(item));
		if (focusable && !m_focus)
		{
			m_focus = id;
		}
		return id;
	}

	void MenuLevel::Link(MenuItemId from, MenuDirection direction, MenuItemId to)
	{
		m_navigation.Link(from, direction, to);
	}

	std::optional<MenuItemId> MenuLevel::Next(MenuItemId from, MenuDirection direction) const
	{
		return m_navigation.Next(from, direction);
	}

	void MenuLevel::SetFocus(MenuItemId id) noexcept
	{
		if (id < m_items.size() && m_items[id].CanFocus())
		{
			m_focus = id;
		}
	}

	RectF MenuLevel::ItemRect(MenuItemId id) const
	{
		return m_layout.Layout(m_items[id].layoutNode);
	}

	std::optional<MenuItemId> MenuLevel::ItemAt(float x, float y) const
	{
		const LayoutNodeId hit = m_layout.HitTest(x, y);
		for (MenuItemId id = 0; id < m_items.size(); ++id)
		{
			if (m_items[id].layoutNode == hit)
			{
				return id;
			}
		}
		return std::nullopt;
	}

	void MenuLevel::Recalculate(float screenWidth, float screenHeight)
	{
		m_layout.Calculate(m_root, screenWidth, screenHeight);
	}

	void MenuLevel::RouteInput(const CommandFrame& commands, const RawKeyboardFrame& keyboard, std::vector<MenuValueChange>& changes)
	{
		for (const MenuLevel* level = this; level != nullptr; level = level->m_parent)
		{
			if (!level->m_focus)
			{
				continue;
			}

			const MenuItemId id = *level->m_focus;
			const auto& control = level->m_items[id].control;
			if (control == nullptr)
			{
				continue;
			}

			std::optional<std::any> change;
			if (control->HandleInput(commands, keyboard, change) == MenuInputOutcome::Handled)
			{
				if (change)
				{
					changes.push_back(MenuValueChange{ id, std::move(*change) });
				}
				return;
			}
		}
	}

	void MenuLevel::Update(double dt)
	{
		for (auto& item : m_items)
		{
			if (item.control != nullptr)
			{
				item.control->Update(dt);
			}
		}
	}

	void MenuLevel::Render(Renderer& renderer, const RectF& screen) const
	{
		if (const auto* background = Background())
		{
			background->Draw(renderer, screen);
		}

		if (m_focus && m_focusGraphic)
		{
			m_focusGraphic->Draw(renderer, ItemRect(*m_focus));
		}

		for (MenuItemId id = 0; id < m_items.size(); ++id)
		{
			if (m_items[id].control != nullptr)
			{
				m_items[id].control->Render(renderer, ItemRect(id));
			}
		}
	}

	bool MenuLevel::FocusedIsTextBox() const noexcept
	{
		return m_focus && dynamic_cast<const MenuTextBox*>(m_items[*m_focus].control.get()) != nullptr;
	}

	void MenuLevel::SetTextFocus(std::optional<MenuItemId> id)
	{
		for (MenuItemId i = 0; i < m_items.size(); ++i)
		{
			if (auto* textBox = dynamic_cast<MenuTextBox*>(m_items[i].control.get()))
			{
				textBox->SetFocused(id && *id == i);
			}
		}
	}

	void MenuLevel::SetTextAssets(const MenuTextAssets& assets)
	{
		for (auto& item : m_items)
		{
			if (auto* textBox = dynamic_cast<MenuTextBox*>(item.control.get()))
			{
				textBox->SetAssets(assets);
			}
		}
	}
}

