//
// MenuItem.h - Single entry in a menu level: command, value, or submenu
//

#pragma once

#include "../Layout/LayoutTree.h"

#include <any>
#include <cstddef>
#include <memory>
#include <string>

namespace Engine
{
	class IMenuControl;

	using MenuItemId = size_t;

	struct MenuValueChange
	{
		MenuItemId item = 0;
		std::any value;
	};

	enum class MenuItemKind
	{
		Command,
		Value,
		Submenu,
	};

	struct MenuItem
	{
		MenuItemKind kind = MenuItemKind::Command;
		std::wstring label;
		LayoutNodeId layoutNode = LayoutTree::kNoHit;
		size_t commandId = 0;
		std::shared_ptr<IMenuControl> control;

		bool CanFocus() const noexcept
		{
			return kind != MenuItemKind::Value || control != nullptr;
		}
	};
}
