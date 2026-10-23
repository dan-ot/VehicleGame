//
// MenuBase.h - Shared menu mode: builds the level tree, owns focus/cursor, and exposes opt-in input consumption
//

#pragma once

#include "../Render/QuadRenderer.h"
#include "../Render/SoftwareCursor.h"
#include "../State/EngineMode.h"
#include "MenuLevel.h"

#include <memory>
#include <optional>
#include <vector>

namespace Engine
{
	struct MenuInputResult
	{
		std::optional<MenuItemId> focusChanged;
		CursorPosition cursor;
		bool accept = false;
		bool abort = false;
		std::vector<MenuValueChange> valueChanges;
	};

	class MenuBase : public IEngineMode
	{
	public:
		explicit MenuBase(AssetKey cursorKey);

		void BindAssets(AssetManager& assets) noexcept { m_assets = &assets; }

		void Render(SharedRenderer& renderer) override;

	protected:
		virtual void BuildRoot(MenuLevel& root) = 0;
		virtual void OnRender(QuadRenderer&, const AssetManager&) {}
		virtual void OnValueChanged(const MenuValueChange&) {}

		void OnEnter() override;

		MenuInputResult ConsumeInput(const CommandFrame& commands, double dt);
		void UpdateLevel(double dt);

		MenuLevel& Root() noexcept { return *m_root; }

	private:
		static constexpr double kRepeatInterval = 0.12;
		static constexpr float kCursorSize = 32.0f;
		static constexpr float kStickSpeedPixels = 900.0f;
		static constexpr float kMousePixelsPerUnit = 1.0f;

		static bool IsActive(const CommandFrame& commands, Command command) noexcept;
		static std::optional<MenuDirection> ActiveDirection(const CommandFrame& commands) noexcept;
		static CommandFrame ResidualCommands(const CommandFrame& commands) noexcept;

		void Step(MenuDirection direction, MenuInputResult& result);
		void FocusItem(MenuItemId id, MenuInputResult& result);
		void CenterCursorOn(MenuItemId id);

		std::unique_ptr<MenuLevel> m_root;
		std::optional<MenuDirection> m_held;
		double m_repeatTimer = 0.0;
		bool m_wasAccept = false;
		bool m_wasAbort = false;
		float m_screenWidth = 0.0f;
		float m_screenHeight = 0.0f;

		AssetManager* m_assets = nullptr;
		std::unique_ptr<QuadRenderer> m_quads;
		ID3D12Device* m_device = nullptr;
		SoftwareCursor m_cursor;
	};
}
