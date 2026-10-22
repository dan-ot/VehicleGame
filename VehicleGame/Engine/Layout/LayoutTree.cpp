//
// LayoutTree.cpp
//

#include "pch.h"
#include "LayoutTree.h"

#include <yoga/Yoga.h>

namespace Engine
{
	namespace
	{
		YGNodeRef ToYoga(void* node)
		{
			return static_cast<YGNodeRef>(node);
		}

		YGJustify ToYogaJustify(LayoutAlign justify)
		{
			switch (justify)
			{
			case LayoutAlign::Center: return YGJustifyCenter;
			case LayoutAlign::End: return YGJustifyFlexEnd;
			default: return YGJustifyFlexStart;
			}
		}

		YGAlign ToYogaAlign(LayoutAlign align)
		{
			switch (align)
			{
			case LayoutAlign::Center: return YGAlignCenter;
			case LayoutAlign::End: return YGAlignFlexEnd;
			case LayoutAlign::Stretch: return YGAlignStretch;
			default: return YGAlignFlexStart;
			}
		}
	}

	LayoutTree::LayoutTree() = default;

	LayoutTree::~LayoutTree()
	{
		for (size_t i = 0; i < m_nodes.size(); ++i)
		{
			if (m_parents[i] == kNoParent)
			{
				YGNodeFree(ToYoga(m_nodes[i]));
			}
		}
	}

	LayoutNodeId LayoutTree::CreateNode()
	{
		m_nodes.push_back(YGNodeNew());
		m_parents.push_back(kNoParent);
		return m_nodes.size() - 1;
	}

	void LayoutTree::AddChild(LayoutNodeId parent, LayoutNodeId child)
	{
		YGNodeInsertChild(ToYoga(m_nodes[parent]), ToYoga(m_nodes[child]), YGNodeGetChildCount(ToYoga(m_nodes[parent])));
		m_parents[child] = parent;
	}

	void LayoutTree::SetFlexDirectionColumn(LayoutNodeId node)
	{
		YGNodeStyleSetFlexDirection(ToYoga(m_nodes[node]), YGFlexDirectionColumn);
	}

	void LayoutTree::SetFlexGrow(LayoutNodeId node, float grow)
	{
		YGNodeStyleSetFlexGrow(ToYoga(m_nodes[node]), grow);
	}

	void LayoutTree::SetFixedSize(LayoutNodeId node, float width, float height)
	{
		YGNodeStyleSetWidth(ToYoga(m_nodes[node]), width);
		YGNodeStyleSetHeight(ToYoga(m_nodes[node]), height);
	}

	void LayoutTree::SetPadding(LayoutNodeId node, float padding)
	{
		YGNodeStyleSetPadding(ToYoga(m_nodes[node]), YGEdgeAll, padding);
	}

	void LayoutTree::SetJustify(LayoutNodeId node, LayoutAlign justify)
	{
		YGNodeStyleSetJustifyContent(ToYoga(m_nodes[node]), ToYogaJustify(justify));
	}

	void LayoutTree::SetAlignItems(LayoutNodeId node, LayoutAlign align)
	{
		YGNodeStyleSetAlignItems(ToYoga(m_nodes[node]), ToYogaAlign(align));
	}

	void LayoutTree::Calculate(LayoutNodeId root, float availableWidth, float availableHeight)
	{
		YGNodeCalculateLayout(ToYoga(m_nodes[root]), availableWidth, availableHeight, YGDirectionLTR);
	}

	RectF LayoutTree::Layout(LayoutNodeId node) const
	{
		RectF rect;
		rect.width = YGNodeLayoutGetWidth(ToYoga(m_nodes[node]));
		rect.height = YGNodeLayoutGetHeight(ToYoga(m_nodes[node]));

		for (LayoutNodeId current = node; current != kNoParent; current = m_parents[current])
		{
			rect.x += YGNodeLayoutGetLeft(ToYoga(m_nodes[current]));
			rect.y += YGNodeLayoutGetTop(ToYoga(m_nodes[current]));
		}
		return rect;
	}
}
