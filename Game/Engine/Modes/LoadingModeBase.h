//
// LoadingModeBase.h - Shared loading splash and staged-asset residency tracker
//

#pragma once

#include "../Assets/AssetKey.h"
#include "../Assets/AssetManager.h"
#include "../Layout/SplashLayout.h"
#include "../Render/QuadRenderer.h"
#include "../Render/RevealedTiledImage.h"
#include "../Render/SoftwareCursor.h"
#include "../Render/SpriteAnimation.h"
#include "../Render/StretchedImage.h"
#include "../State/EngineMode.h"
#include "../State/LoadingTracker.h"
#include "ModeDescriptors.h"

#include <algorithm>
#include <atomic>
#include <memory>
#include <optional>
#include <vector>

namespace Engine
{
	class LoadingModeBase : public IEngineMode
	{
	public:
		void BindAssets(AssetManager& assets) noexcept { m_assets = &assets; }
		void BindStagedManifest(const AssetManifest& manifest) noexcept { m_stagedManifest = &manifest; }

		// Marks the transition as failed; the mode then falls back to the Main Menu.
		void MarkFailed() noexcept { m_failed = true; }
		bool Failed() const noexcept { return m_failed; }

		// No progress driver yet; callers set the value directly.
		void SetLoadingProgress(float percent) noexcept { m_loadingBar.SetPercent(percent); }
		float LoadingProgress() const noexcept { return m_loadingBar.Percent(); }

		void Render(SharedRenderer& renderer) override
		{
			m_screenWidth = renderer.ViewportWidth();
			m_screenHeight = renderer.ViewportHeight();

			m_cursor.EnsurePlaced(m_screenWidth, m_screenHeight);

			m_layout.Recalculate(m_screenWidth, m_screenHeight);
			const RectF screen{ 0.0f, 0.0f, m_screenWidth, m_screenHeight };
			m_background.Draw(renderer, screen);
			m_logo.Draw(renderer, m_layout.Logo());
			m_loadingBar.Draw(renderer, m_layout.LoadingBar());
			m_spinner.Draw(renderer, m_layout.Spinner());
			m_cursor.Render(renderer);
		}

	protected:
		explicit LoadingModeBase(ModeId defaultDestination)
			: m_background(TextureKey(kSplashPath))
			, m_logo(TextureKey(kLogoPath))
			, m_loadingBar(TextureKey(kBarFillPath), kBarTileWidth, kBarTileHeight)
			, m_spinner(TextureKey(kSpinnerPath), kSpinnerColumns, kSpinnerRows, kSpinnerFrames, kSpinnerFrameSeconds)
			, m_cursor(TextureKey(kCursorPath), kCursorSize)
			, m_defaultDestination(defaultDestination)
		{
			m_placeholderWork = m_tracker.AddWork();
			m_cursor.SetScaling(CursorScaling{ kStickSpeedPixels, kMousePixelsPerUnit });
		}

		void DeclareAssets(AssetManifest& manifest) override
		{
			ModeDescriptors::Loading::DeclareAssets(manifest);
		}

		void OnEnter() override
		{
			m_tracker = LoadingTracker{};
			m_placeholderWork = m_tracker.AddWork();
			m_assetWork.clear();
			m_failed = false;
			if (m_stagedManifest)
			{
				for (const AssetKey& key : m_stagedManifest->Keys())
				{
					m_assetWork.push_back(AssetWork{ key, m_tracker.AddWork() });
				}
			}
			m_destination = m_defaultDestination;
			m_pendingHandoff.reset();
			if (const auto handoff = TakeHandoff())
			{
				m_destination = handoff->target;
				m_pendingHandoff = handoff;
			}
		}

		void Update(const ModeTick& tick, const CommandFrame& commands, const RawKeyboardFrame& keyboard) override
		{
			for (const AssetWork& work : m_assetWork)
			{
				if (m_assets->HasFailed(work.key))
				{
					m_failed = true;
				}
				else if (!m_failed && m_assets->IsBuilt(work.key))
				{
					m_tracker.Complete(work.id);
				}
			}

			// Minimum display time; the tracker otherwise reflects real asset residency.
			m_tracker.Report(m_placeholderWork, Elapsed() / kLoadSeconds);
			m_loadingBar.SetPercent(static_cast<float>(m_tracker.Aggregate()));

			if (m_failed)
			{
				m_pendingHandoff.reset();
				RequestTransition(ModeId::MainMenu);
			}
			else if (m_tracker.IsComplete())
			{
				if (m_pendingHandoff)
				{
					PublishHandoff(*m_pendingHandoff);
				}
				RequestTransition(m_destination);
			}

			m_spinner.Update(tick.modeDt);
			m_cursor.ProcessInput(commands, tick.modeDt, m_screenWidth, m_screenHeight);
			m_cursor.Update(tick.modeDt);
		}

	private:
		static constexpr double kLoadSeconds = 2.0;

		static constexpr const wchar_t* kSplashPath = L"Assets\\loading_splash.tga";
		static constexpr const wchar_t* kLogoPath = L"Assets\\loading_logo.tga";
		static constexpr const wchar_t* kBarFillPath = L"Assets\\loading_bar_fill.tga";
		static constexpr const wchar_t* kSpinnerPath = L"Assets\\loading_spinner.tga";
		static constexpr const wchar_t* kCursorPath = L"Assets\\cursor.tga";

		static constexpr float kBarTileWidth = 64.0f;
		static constexpr float kBarTileHeight = 24.0f;
		static constexpr uint32_t kSpinnerColumns = 4;
		static constexpr uint32_t kSpinnerRows = 4;
		static constexpr uint32_t kSpinnerFrames = 16;
		static constexpr double kSpinnerFrameSeconds = 1.0 / 15.0;

		static constexpr float kCursorSize = 32.0f;
		static constexpr float kStickSpeedPixels = 900.0f;
		static constexpr float kMousePixelsPerUnit = 1.0f;

		static AssetKey TextureKey(const wchar_t* path)
		{
			return MakeAssetKey(AssetType::Texture, path);
		}

		AssetManager* m_assets = nullptr;

		SplashLayout m_layout;
		float m_screenWidth = 0.0f;
		float m_screenHeight = 0.0f;

		StretchedImage m_background;
		StretchedImage m_logo;
		RevealedTiledImage m_loadingBar;
		SpriteAnimation m_spinner;
		SoftwareCursor m_cursor;

		struct AssetWork
		{
			AssetKey key;
			std::size_t id;
		};

		const AssetManifest* m_stagedManifest = nullptr;
		std::vector<AssetWork> m_assetWork;

		LoadingTracker m_tracker;
		std::size_t m_placeholderWork = 0;
		ModeId m_defaultDestination = ModeId::MainMenu;
		ModeId m_destination = ModeId::MainMenu;
		bool m_failed = false;
		std::optional<ModeHandoff> m_pendingHandoff;
	};
}
