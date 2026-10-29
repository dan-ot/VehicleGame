//
// ModeDescriptors.h - Per-mode metadata and asset manifests, queryable without constructing the mode
//

#pragma once

#include "../Assets/AssetManifest.h"
#include "../State/ModeId.h"

namespace Engine
{
	namespace ModeDescriptors
	{
		namespace Paths
		{
			inline constexpr const wchar_t* kSplash = L"Assets\\loading_splash.tga";
			inline constexpr const wchar_t* kLogo = L"Assets\\loading_logo.tga";
			inline constexpr const wchar_t* kBarFill = L"Assets\\loading_bar_fill.tga";
			inline constexpr const wchar_t* kSpinner = L"Assets\\loading_spinner.tga";
			inline constexpr const wchar_t* kCursor = L"Assets\\cursor.tga";
			inline constexpr const wchar_t* kMenuBackground = L"Assets\\menu_background.tga";
			inline constexpr const wchar_t* kWhite = L"Assets\\white_pixel.tga";
			inline constexpr const wchar_t* kFont = L"Assets\\menu_font.ttf";
			inline constexpr const wchar_t* kClick = L"Assets\\menu_click.wav";
			inline constexpr const wchar_t* kVehicle = L"Assets\\vehicle.glb";
			inline constexpr const wchar_t* kEngine = L"Assets\\engine.wav";
		}

		// Shared by every loading variant; the staged destination decides what follows.
		struct Loading
		{
			static void DeclareAssets(AssetManifest& manifest)
			{
				manifest.Add(AssetType::Texture, Paths::kSplash);
				manifest.Add(AssetType::Texture, Paths::kLogo);
				manifest.Add(AssetType::Texture, Paths::kBarFill);
				manifest.Add(AssetType::Texture, Paths::kSpinner);
				manifest.Add(AssetType::Texture, Paths::kCursor);
			}
		};

		struct MainMenu
		{
			static constexpr ModeId kId = ModeId::MainMenu;

			static void DeclareAssets(AssetManifest& manifest)
			{
				manifest.Add(AssetType::Texture, Paths::kMenuBackground);
				manifest.Add(AssetType::Texture, Paths::kCursor);
				manifest.Add(AssetType::Texture, Paths::kWhite);
				manifest.Add(AssetType::Font, Paths::kFont);
				manifest.Add(AssetType::Sound, Paths::kClick);
			}
		};

		struct LobbyPlay
		{
			static constexpr ModeId kId = ModeId::LobbyPlay;

			static void DeclareAssets(AssetManifest& manifest)
			{
				manifest.Add(AssetType::Texture, Paths::kMenuBackground);
				manifest.Add(AssetType::Texture, Paths::kCursor);
				manifest.Add(AssetType::Texture, Paths::kWhite);
				manifest.Add(AssetType::Font, Paths::kFont);
				manifest.Add(AssetType::Sound, Paths::kClick);
				manifest.Add(AssetType::Model, Paths::kVehicle);
				manifest.Add(AssetType::Sound, Paths::kEngine);
			}
		};

		inline AssetManifest ManifestFor(ModeId mode)
		{
			AssetManifest manifest;
			switch (mode)
			{
			case ModeId::SplashScreen:
			case ModeId::MenuLoading:
			case ModeId::LobbyLoading: Loading::DeclareAssets(manifest); break;
			case ModeId::MainMenu:     MainMenu::DeclareAssets(manifest); break;
			case ModeId::LobbyPlay:    LobbyPlay::DeclareAssets(manifest); break;
			}
			return manifest;
		}
	}
}
