//
// LoadingTracker.h
//

#pragma once

#include <algorithm>
#include <cstddef>
#include <vector>

namespace Engine
{
	class LoadingTracker
	{
	public:
		std::size_t AddWork(double weight = 1.0)
		{
			m_items.push_back(Item{ weight > 0.0 ? weight : 0.0, 0.0 });
			return m_items.size() - 1;
		}

		void Report(std::size_t id, double fraction) noexcept
		{
			if (id < m_items.size())
			{
				m_items[id].fraction = std::clamp(fraction, 0.0, 1.0);
			}
		}

		void Complete(std::size_t id) noexcept { Report(id, 1.0); }

		double Aggregate() const noexcept
		{
			double totalWeight = 0.0;
			double done = 0.0;
			for (const auto& item : m_items)
			{
				totalWeight += item.weight;
				done += item.weight * item.fraction;
			}
			return totalWeight > 0.0 ? done / totalWeight : 1.0;
		}

		bool IsComplete() const noexcept
		{
			return std::all_of(m_items.begin(), m_items.end(), [](const Item& item) { return item.fraction >= 1.0; });
		}

	private:
		struct Item
		{
			double weight;
			double fraction;
		};

		std::vector<Item> m_items;
	};
}
