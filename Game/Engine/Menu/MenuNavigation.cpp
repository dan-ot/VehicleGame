//
// MenuNavigation.cpp
//

#include "pch.h"
#include "MenuNavigation.h"

namespace Engine
{
	void MenuNavigation::Link(MenuItemId from, MenuDirection direction, MenuItemId to)
	{
		if (m_neighbors.size() <= from)
		{
			m_neighbors.resize(from + 1);
		}
		m_neighbors[from][static_cast<size_t>(direction)] = to;
	}

	std::optional<MenuItemId> MenuNavigation::Next(MenuItemId from, MenuDirection direction) const
	{
		if (from >= m_neighbors.size())
		{
			return std::nullopt;
		}
		return m_neighbors[from][static_cast<size_t>(direction)];
	}
}
