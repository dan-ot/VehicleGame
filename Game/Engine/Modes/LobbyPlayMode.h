//
// LobbyPlayMode.h - Lobby with a menu canvas over a separate world Scene
//

#pragma once

#include "../Menu/MenuBase.h"
#include "../Scene/Scene.h"
#include "../State/ModeHandoff.h"
#include "ModeDescriptors.h"

#include <string>
#include <variant>

namespace Engine
{
	class LobbyPlayMode final : public MenuBase
	{
	public:
		LobbyPlayMode()
			: MenuBase(CursorKey())
		{
		}

	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			ModeDescriptors::LobbyPlay::DeclareAssets(manifest);
		}

		void OnEnter() override
		{
			MenuBase::OnEnter();
			if (const auto handoff = TakeHandoff())
			{
				if (const auto* play = std::get_if<PlayHandoff>(&handoff->payload))
				{
					m_levelName = play->levelName;
				}
			}
		}

		void BuildRoot(MenuLevel& root) override
		{
			root.SetBackground(BackgroundKey());
			root.AddItem(MenuItem{ MenuItemKind::Command, L"Leave Lobby" }, kItemWidth, kItemHeight);
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

			const auto selected = Root().Focus();
			if (commands[Command::System_Abort].greatest.valid || (result.accept && selected && *selected == kLeaveLobby))
			{
				PublishHandoff(ModeHandoff{ ModeId::MainMenu, MenuHandoff{} });
				RequestTransition(ModeId::MenuLoading);
				return;
			}

			m_scene.Update(tick.modeDt);
		}

	private:
		static constexpr MenuItemId kLeaveLobby = 0;

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

		Scene m_scene;
		std::wstring m_levelName;
	};
}
