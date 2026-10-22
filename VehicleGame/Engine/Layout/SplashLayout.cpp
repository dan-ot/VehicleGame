//
// SplashLayout.cpp
//

#include "pch.h"
#include "SplashLayout.h"

namespace Engine
{
	namespace
	{
		constexpr float kLogoWidth = 640.0f;
		constexpr float kLogoHeight = 360.0f;
		constexpr float kBarWidth = 600.0f;
		constexpr float kBarHeight = 24.0f;
		constexpr float kSpinnerSize = 96.0f;
	}

	SplashLayout::SplashLayout()
	{
		m_root = m_tree.CreateNode();
		m_logo = m_tree.CreateNode();
		m_loadingBar = m_tree.CreateNode();
		m_spinner = m_tree.CreateNode();

		m_tree.SetFlexDirectionColumn(m_root);
		m_tree.SetJustify(m_root, LayoutAlign::Center);
		m_tree.SetAlignItems(m_root, LayoutAlign::Center);

		m_tree.SetFixedSize(m_logo, kLogoWidth, kLogoHeight);
		m_tree.SetFixedSize(m_loadingBar, kBarWidth, kBarHeight);
		m_tree.SetFixedSize(m_spinner, kSpinnerSize, kSpinnerSize);

		m_tree.AddChild(m_root, m_logo);
		m_tree.AddChild(m_root, m_loadingBar);
		m_tree.AddChild(m_root, m_spinner);
	}

	void SplashLayout::Recalculate(float screenWidth, float screenHeight)
	{
		m_tree.Calculate(m_root, screenWidth, screenHeight);
	}

	RectF SplashLayout::Logo() const
	{
		return m_tree.Layout(m_logo);
	}

	RectF SplashLayout::LoadingBar() const
	{
		return m_tree.Layout(m_loadingBar);
	}

	RectF SplashLayout::Spinner() const
	{
		return m_tree.Layout(m_spinner);
	}
}
