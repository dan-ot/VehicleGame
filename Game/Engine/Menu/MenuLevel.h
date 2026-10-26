//
// MenuLevel.h - One level of a menu tree: items, layout, navigation, focus, and background
//

#pragma once

#include "../Layout/LayoutTree.h"
#include "../Render/StretchedImage.h"
#include "../State/InputCommands.h"
#include "../State/RawKeyboardFrame.h"
#include "MenuItem.h"
#include "MenuNavigation.h"
#include "MenuTextBox.h"

#include <optional>
#include <vector>

namespace Engine
{
	class MenuLevel final
	{
	public:
		explicit MenuLevel(MenuLevel* parent = nullptr);

		MenuLevel(const MenuLevel&) = delete;
		MenuLevel& operator=(const MenuLevel&) = delete;

		void SetBackground(AssetKey key);
		void SetFocusGraphic(AssetKey key);
		const StretchedImage* Background() const noexcept;

		MenuItemId AddItem(MenuItem item, float width, float height);
		void Link(MenuItemId from, MenuDirection direction, MenuItemId to);
		std::optional<MenuItemId> Next(MenuItemId from, MenuDirection direction) const;

		size_t ItemCount() const noexcept { return m_items.size(); }
		const MenuItem& Item(MenuItemId id) const { return m_items[id]; }
		MenuItem& Item(MenuItemId id) { return m_items[id]; }

		std::optional<MenuItemId> Focus() const noexcept { return m_focus; }
		void SetFocus(MenuItemId id) noexcept;

		RectF ItemRect(MenuItemId id) const;
		std::optional<MenuItemId> ItemAt(float x, float y) const;

		MenuLevel* Parent() const noexcept { return m_parent; }
		void RouteInput(const CommandFrame& commands, const RawKeyboardFrame& keyboard, std::vector<MenuValueChange>& changes);

		void SetTextAssets(const MenuTextAssets& assets);
		void SetTextFocus(std::optional<MenuItemId> id);
		bool FocusedIsTextBox() const noexcept;

		void Recalculate(float screenWidth, float screenHeight);
		void Update(double dt);
		void Render(QuadRenderer& renderer, const AssetManager& assets, const RectF& screen) const;

	private:
		MenuLevel* m_parent = nullptr;
		LayoutTree m_layout;
		LayoutNodeId m_root = LayoutTree::kNoHit;
		std::vector<MenuItem> m_items;
		MenuNavigation m_navigation;
		std::optional<MenuItemId> m_focus;
		std::optional<StretchedImage> m_background;
		std::optional<StretchedImage> m_focusGraphic;
	};
}
