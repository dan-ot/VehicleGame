//
// LoadingMode.h
//

#pragma once

#include "../Assets/AssetKey.h"
#include "../Assets/AssetManager.h"
#include "../Layout/SplashLayout.h"
#include "../Render/QuadRenderer.h"
#include "../Render/RevealedTiledImage.h"
#include "../Render/SpriteAnimation.h"
#include "../Render/StretchedImage.h"
#include "../State/EngineMode.h"
#include "../State/LoadingTracker.h"

#include <algorithm>
#include <atomic>
#include <memory>
#include <optional>
#include <vector>

namespace Engine
{
	class LoadingMode final : public IEngineMode
	{
	public:
		LoadingMode()
			: m_background(TextureKey(kSplashPath))
			, m_logo(TextureKey(kLogoPath))
			, m_loadingBar(TextureKey(kBarFillPath), kBarTileWidth, kBarTileHeight)
			, m_spinner(TextureKey(kSpinnerPath), kSpinnerColumns, kSpinnerRows, kSpinnerFrames, kSpinnerFrameSeconds)
			, m_cursor(TextureKey(kCursorPath))
		{
			m_placeholderWork = m_tracker.AddWork();
		}

		void BindAssets(AssetManager& assets) noexcept { m_assets = &assets; }

		// No progress driver yet; callers set the value directly.
		void SetLoadingProgress(float percent) noexcept { m_loadingBar.SetPercent(percent); }
		float LoadingProgress() const noexcept { return m_loadingBar.Percent(); }

		void Render(SharedRenderer& renderer) override
		{
			const auto viewport = renderer.GetScreenViewport();
			m_screenWidth = viewport.Width;
			m_screenHeight = viewport.Height;

			if (m_quads == nullptr || m_device != renderer.GetD3DDevice())
			{
				m_quads = std::make_unique<QuadRenderer>(renderer);
				m_device = renderer.GetD3DDevice();
			}

			if (!m_cursorPlaced)
			{
				m_cursorX = m_screenWidth * 0.5f;
				m_cursorY = m_screenHeight * 0.5f;
				m_cursorPlaced = true;
			}

			{
				return;
			}

			m_layout.Recalculate(m_screenWidth, m_screenHeight);
			const RectF screen{ 0.0f, 0.0f, m_screenWidth, m_screenHeight };
			m_background.Draw(*m_quads, *m_assets, screen);
			m_logo.Draw(*m_quads, *m_assets, m_layout.Logo());
			m_loadingBar.Draw(*m_quads, *m_assets, m_layout.LoadingBar());
			m_spinner.Draw(*m_quads, *m_assets, m_layout.Spinner());
			m_cursor.Draw(*m_quads, *m_assets, RectF{ m_cursorX, m_cursorY, kCursorSize, kCursorSize });
		}

	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			manifest.Add(AssetType::Texture, kSplashPath);
			manifest.Add(AssetType::Texture, kLogoPath);
			manifest.Add(AssetType::Texture, kBarFillPath);
			manifest.Add(AssetType::Texture, kSpinnerPath);
			manifest.Add(AssetType::Texture, kCursorPath);
		}

		void OnEnter() override
		{
			m_tracker = LoadingTracker{};
			m_placeholderWork = m_tracker.AddWork();
			m_assetWork.clear();
			for (const AssetKey& key : m_assetKeys)
			{
				m_assetWork.push_back(AssetWork{ key, m_tracker.AddWork() });
			}
			m_destination = EngineState::Menu;
			m_pendingHandoff.reset();
			if (const auto handoff = TakeHandoff())
			{
				m_destination = handoff->target;
				m_pendingHandoff = handoff;
			}
		}

		void Update(const ModeTick& tick, const CommandFrame& commands) override
		{
			for (const AssetWork& work : m_assetWork)
			{
				if (m_assets != nullptr && m_assets->Find(work.key) != nullptr)
				{
					m_tracker.Complete(work.id);
				}
			}

			// Minimum display time; the tracker otherwise reflects real asset residency.
			m_tracker.Report(m_placeholderWork, Elapsed() / kLoadSeconds);
			m_loadingBar.SetPercent(static_cast<float>(m_tracker.Aggregate()));

			if (m_tracker.IsComplete())
			{
				if (m_pendingHandoff)
				{
					PublishHandoff(*m_pendingHandoff);
				}
				RequestTransition(m_destination);
			}

			m_spinner.Update(tick.modeDt);
			MoveCursor(tick.modeDt, commands);
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

		static float StackStrength(const CommandValue& value, InputDevice device) noexcept
		{
			const auto& contribution = value.stack[static_cast<std::size_t>(device)];
			return contribution ? contribution->strength : 0.0f;
		}

		// Controller strength is normalized and scaled by time; mouse strength is a raw per-frame delta.
		void MoveCursor(double dt, const CommandFrame& commands)
		{
			const auto& left = commands[Command::Navigate_Left];
			const auto& right = commands[Command::Navigate_Right];
			const auto& up = commands[Command::Navigate_Up];
			const auto& down = commands[Command::Navigate_Down];

			const float stickX = StackStrength(right, InputDevice::Controller) - StackStrength(left, InputDevice::Controller);
			const float stickY = StackStrength(down, InputDevice::Controller) - StackStrength(up, InputDevice::Controller);
			const float mouseX = StackStrength(right, InputDevice::Mouse) - StackStrength(left, InputDevice::Mouse);
			const float mouseY = StackStrength(down, InputDevice::Mouse) - StackStrength(up, InputDevice::Mouse);

			const float stepSeconds = static_cast<float>(dt);
			m_cursorX += stickX * kStickSpeedPixels * stepSeconds + mouseX * kMousePixelsPerUnit;
			m_cursorY += stickY * kStickSpeedPixels * stepSeconds + mouseY * kMousePixelsPerUnit;

			const float maxX = std::max(0.0f, m_screenWidth - kCursorSize);
			const float maxY = std::max(0.0f, m_screenHeight - kCursorSize);
			m_cursorX = std::clamp(m_cursorX, 0.0f, maxX);
			m_cursorY = std::clamp(m_cursorY, 0.0f, maxY);
		}

		AssetManager* m_assets = nullptr;
		std::unique_ptr<QuadRenderer> m_quads;
		ID3D12Device* m_device = nullptr;

		SplashLayout m_layout;
		float m_screenWidth = 0.0f;
		float m_screenHeight = 0.0f;

		StretchedImage m_background;
		StretchedImage m_logo;
		RevealedTiledImage m_loadingBar;
		SpriteAnimation m_spinner;
		StretchedImage m_cursor;

		float m_cursorX = 0.0f;
		float m_cursorY = 0.0f;
		bool m_cursorPlaced = false;

		struct AssetWork
		{
			AssetKey key;
			std::size_t id;
		};

		const std::vector<AssetKey> m_assetKeys{ TextureKey(kSplashPath), TextureKey(kLogoPath), TextureKey(kBarFillPath), TextureKey(kSpinnerPath), TextureKey(kCursorPath) };
		std::vector<AssetWork> m_assetWork;

		LoadingTracker m_tracker;
		std::size_t m_placeholderWork = 0;
		EngineState m_destination = EngineState::Menu;
		std::optional<ModeHandoff> m_pendingHandoff;
	};
}
