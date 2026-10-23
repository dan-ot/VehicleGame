//
// MenuNavigation.h - One-way neighbor table used for Direction_* focus movement
//

#pragma once

#include "MenuItem.h"

#include <array>
#include <optional>
#include <vector>

namespace Engine
{
	enum class MenuDirection : size_t
	{
		Up,
		Down,
		Left,
		Right,
		Count
	};

	class MenuNavigation
	{
	public:
		void Link(MenuItemId from, MenuDirection direction, MenuItemId to);
		std::optional<MenuItemId> Next(MenuItemId from, MenuDirection direction) const;

	private:
		using Neighbors = std::array<std::optional<MenuItemId>, static_cast<size_t>(MenuDirection::Count)>;

		std::vector<Neighbors> m_neighbors;
	};
}
