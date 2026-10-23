//
// MenuItem.h - Single entry in a menu level: command, value, or submenu
//

#pragma once

#include "../Layout/LayoutTree.h"

#include <cstddef>
#include <string>

namespace Engine
{
	class IMenuControl;

	using MenuItemId = size_t;

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
		IMenuControl* control = nullptr;

		bool CanFocus() const noexcept
		{
			return kind != MenuItemKind::Value || control != nullptr;
		}
	};
}
