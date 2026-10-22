//
// SplashLayout.h - Hardcoded Yoga layout for the loading splash screen
//

#pragma once

#include "LayoutTree.h"

namespace Engine
{
	class SplashLayout
	{
	public:
		SplashLayout();

		SplashLayout(const SplashLayout&) = delete;
		SplashLayout& operator=(const SplashLayout&) = delete;

		void Recalculate(float screenWidth, float screenHeight);

		RectF Logo() const;
		RectF LoadingBar() const;
		RectF Spinner() const;

	private:
		LayoutTree m_tree;
		LayoutNodeId m_root;
		LayoutNodeId m_logo;
		LayoutNodeId m_loadingBar;
		LayoutNodeId m_spinner;
	};
}
