//
// MenuMode.h
//

#pragma once

#include "../Menu/MenuBase.h"

namespace Engine
{
	class MenuMode final : public MenuBase
	{
	public:
		MenuMode()
			: MenuBase(CursorKey())
		{
		}

	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			manifest.Add(AssetType::Texture, kBackgroundPath);
			manifest.Add(AssetType::Texture, kCursorPath);
			manifest.Add(AssetType::Texture, kWhitePath);
			manifest.Add(AssetType::Font, kFontPath);
			manifest.Add(AssetType::Sound, L"Assets\\menu_click.wav");
		}

		void BuildRoot(MenuLevel& root) override
		{
			root.SetBackground(BackgroundKey());
			root.AddItem(MenuItem{ MenuItemKind::Command, L"Play" }, kItemWidth, kItemHeight);
			root.AddItem(MenuItem{ MenuItemKind::Command, L"Options" }, kItemWidth, kItemHeight);
			root.AddItem(MenuItem{ MenuItemKind::Value, L"Name", LayoutTree::kNoHit, 0, std::make_shared<MenuTextBox>(MenuTextMode::FreeText) }, kItemWidth, kItemHeight);
			root.Link(0, MenuDirection::Down, 1);
			root.Link(1, MenuDirection::Up, 0);
			root.Link(1, MenuDirection::Down, 2);
			root.Link(2, MenuDirection::Up, 1);
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

			if (*selected == 0)
			{
				PublishHandoff(ModeHandoff{ EngineState::Play, PlayHandoff{} });
			}
			else
			{
				PublishHandoff(ModeHandoff{ EngineState::Menu, MenuHandoff{ static_cast<int>(*selected) } });
			}
			RequestTransition(EngineState::Loading);
		}

	private:
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
