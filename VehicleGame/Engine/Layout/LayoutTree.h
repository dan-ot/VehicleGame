//
// LayoutTree.h - Flexbox layout wrapper over Yoga; Yoga types stay in LayoutTree.cpp
//

#pragma once

#include "../Render/Primitives.h"

#include <cstddef>
#include <vector>

namespace Engine
{
	using LayoutNodeId = size_t;

	enum class LayoutAlign
	{
		Start,
		Center,
		End,
		Stretch,
	};

	class LayoutTree
	{
	public:
		LayoutTree();
		~LayoutTree();

		LayoutTree(const LayoutTree&) = delete;
		LayoutTree& operator=(const LayoutTree&) = delete;

		LayoutNodeId CreateNode();
		void AddChild(LayoutNodeId parent, LayoutNodeId child);

		void SetFlexDirectionColumn(LayoutNodeId node);
		void SetFlexGrow(LayoutNodeId node, float grow);
		void SetFixedSize(LayoutNodeId node, float width, float height);
		void SetPadding(LayoutNodeId node, float padding);
		void SetJustify(LayoutNodeId node, LayoutAlign justify);
		void SetAlignItems(LayoutNodeId node, LayoutAlign align);

		// Computes layout for the tree rooted at root within the given available size.
		void Calculate(LayoutNodeId root, float availableWidth, float availableHeight);

		// Returns the node's computed rect in the coordinate space of the root passed to Calculate.
		RectF Layout(LayoutNodeId node) const;

	private:
		static constexpr LayoutNodeId kNoParent = static_cast<LayoutNodeId>(-1);

		std::vector<void*> m_nodes;
		std::vector<LayoutNodeId> m_parents;
	};
}
