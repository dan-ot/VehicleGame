//
// MainMenuMode.h - Top-level menu: Continue, New Game, Saved Games, Settings, and player name
//

#pragma once

#include "../Menu/MenuBase.h"
#include "../State/ModeHandoff.h"
#include "ModeDescriptors.h"

namespace Engine
{
	class MainMenuMode final : public MenuBase
	{
	public:
		MainMenuMode()
			: MenuBase(CursorKey())
		{
		}

	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			ModeDescriptors::MainMenu::DeclareAssets(manifest);
		}

		void BuildRoot(MenuLevel& root) override
		{
			root.SetBackground(BackgroundKey());
			root.AddItem(MenuItem{ MenuItemKind::Command, L"Continue" }, kItemWidth, kItemHeight);
			root.AddItem(MenuItem{ MenuItemKind::Command, L"New Game" }, kItemWidth, kItemHeight);
			root.AddItem(MenuItem{ MenuItemKind::Command, L"Saved Games" }, kItemWidth, kItemHeight);
			root.AddItem(MenuItem{ MenuItemKind::Command, L"Settings" }, kItemWidth, kItemHeight);
			root.AddItem(MenuItem{ MenuItemKind::Value, L"Name", LayoutTree::kNoHit, 0, std::make_shared<MenuTextBox>(MenuTextMode::FreeText) }, kItemWidth, kItemHeight);
			for (MenuItemId id = 0; id + 1 < root.ItemCount(); ++id)
			{
				root.Link(id, MenuDirection::Down, id + 1);
				root.Link(id + 1, MenuDirection::Up, id);
			}
		}

		void OnRender(Renderer& renderer) override
		{
			MenuLevel& root = Root();
			for (MenuItemId id = 0; id < root.ItemCount(); ++id)
			{
				const ColorF tint = root.Focus() == id ? kSelectedTint : kUnselectedTint;
				renderer.Draw(BackgroundKey(), root.ItemRect(id), UvRectF{}, tint);
			}
		}

		void Update(const ModeTick& tick, const CommandFrame& commands, const RawKeyboardFrame& keyboard) override
		{
			UpdateLevel(tick.modeDt);

			const MenuInputResult result = ConsumeInput(commands, keyboard, tick.modeDt);
			for (const auto& change : result.valueChanges)
			{
				OnValueChanged(change);
			}

			if (!result.accept)
			{
				return;
			}

			const auto selected = Root().Focus();
			if (!selected || Root().Item(*selected).kind == MenuItemKind::Value)
			{
				return;
			}

			// TODO: Continue, Saved Games, and Settings are placeholders; Settings controls are note-only for now.
			if (*selected == kNewGame)
			{
				PublishHandoff(ModeHandoff{ ModeId::LobbyPlay, PlayHandoff{} });
				RequestTransition(ModeId::LobbyLoading);
			}
		}

	private:
		static constexpr MenuItemId kNewGame = 1;

		static constexpr const wchar_t* kBackgroundPath = L"Assets\\menu_background.tga";
		static constexpr const wchar_t* kCursorPath = L"Assets\\cursor.tga";
		static constexpr const wchar_t* kWhitePath = L"Assets\\white_pixel.tga";
		static constexpr const wchar_t* kFontPath = L"Assets\\menu_font.ttf";
		static constexpr float kItemWidth = 240.0f;
		static constexpr float kItemHeight = 48.0f;
		static constexpr ColorF kSelectedTint{ 1.0f, 0.9f, 0.3f, 1.0f };
		static constexpr ColorF kUnselectedTint{ 0.35f, 0.35f, 0.35f, 1.0f };

		static AssetKey BackgroundKey()
		{
			return MakeAssetKey(AssetType::Texture, kBackgroundPath);
		}

		static AssetKey CursorKey()
		{
			return MakeAssetKey(AssetType::Texture, kCursorPath);
		}

		AssetKey TextFontKey() const override
		{
			return MakeAssetKey(AssetType::Font, kFontPath);
		}

		AssetKey WhiteTextureKey() const override
		{
			return MakeAssetKey(AssetType::Texture, kWhitePath);
		}
	};
}
