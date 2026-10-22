//
// MenuMode.h
//

#pragma once

#include "../Render/QuadRenderer.h"
#include "../Render/StretchedImage.h"
#include "../State/EngineMode.h"

#include <memory>

namespace Engine
{
	class MenuMode final : public IEngineMode
	{
	public:
		MenuMode()
			: m_background(BackgroundKey())
		{
		}

		void BindAssets(AssetManager& assets) noexcept { m_assets = &assets; }

		void Render(SharedRenderer& renderer) override
		{
			const auto viewport = renderer.GetScreenViewport();
			if (m_quads == nullptr || m_device != renderer.GetD3DDevice())
			{
				m_quads = std::make_unique<QuadRenderer>(renderer);
				m_device = renderer.GetD3DDevice();
			}

			if (m_assets == nullptr)
			{
				return;
			}

			const RectF screen{ 0.0f, 0.0f, viewport.Width, viewport.Height };
			m_background.Draw(*m_quads, *m_assets, screen);

			const auto* texture = FindResidentTexture(*m_assets, BackgroundKey());
			if (texture == nullptr)
			{
				return;
			}

			const float x = viewport.Width * 0.5f - kItemWidth * 0.5f;
			for (int i = 0; i < kItemCount; ++i)
			{
				const float y = viewport.Height * (kFirstItemY + kItemSpacingY * static_cast<float>(i));
				const ColorF tint = i == m_selected ? kSelectedTint : kUnselectedTint;
				m_quads->Draw(texture->GpuDescriptor(), RectF{ x, y, kItemWidth, kItemHeight }, UvRectF{}, tint);
			}
		}

	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			manifest.Add(AssetType::Texture, kBackgroundPath);
			manifest.Add(AssetType::Sound, L"Assets\\menu_click.wav");
		}

		void Update(const ModeTick& tick, const CommandFrame& commands) override
		{
			const int direction = Direction(commands);
			if (direction == 0)
			{
				m_repeatTimer = 0.0;
				m_held = 0;
			}
			else if (direction != m_held)
			{
				m_held = direction;
				m_repeatTimer = 0.0;
				Step(direction);
			}
			else
			{
				m_repeatTimer += tick.modeDt;
				if (m_repeatTimer >= kRepeatInterval)
				{
					m_repeatTimer -= kRepeatInterval;
					Step(direction);
				}
			}

			if (IsActive(commands, Command::System_Accept) && !m_wasAccept)
			{
				if (m_selected == 0)
				{
					PublishHandoff(ModeHandoff{ EngineState::Play, PlayHandoff{} });
					RequestTransition(EngineState::Loading);
				}
				else
				{
					PublishHandoff(ModeHandoff{ EngineState::Menu, MenuHandoff{ m_selected } });
					RequestTransition(EngineState::Loading);
				}
			}
			m_wasAccept = IsActive(commands, Command::System_Accept);
		}

	private:
		static bool IsActive(const CommandFrame& commands, Command command) noexcept
		{
			return commands[command].greatest.valid && commands[command].greatest.strength >= 0.5f;
		}

		static int Direction(const CommandFrame& commands) noexcept
		{
			if (IsActive(commands, Command::Direction_Up) || IsActive(commands, Command::Movement_Up))
			{
				return -1;
			}
			if (IsActive(commands, Command::Direction_Down) || IsActive(commands, Command::Movement_Down))
			{
				return 1;
			}
			return 0;
		}

		void Step(int direction) noexcept
		{
			const int next = m_selected + direction;
			if (next >= 0 && next < kItemCount)
			{
				m_selected = next;
			}
		}

		static constexpr int kItemCount = 2;
		static constexpr double kRepeatInterval = 0.12;

		static constexpr const wchar_t* kBackgroundPath = L"Assets\\menu_background.tga";
		static constexpr float kFirstItemY = 0.45f;
		static constexpr float kItemSpacingY = 0.12f;
		static constexpr float kItemWidth = 240.0f;
		static constexpr float kItemHeight = 48.0f;
		static constexpr ColorF kSelectedTint{ 1.0f, 0.9f, 0.3f, 1.0f };
		static constexpr ColorF kUnselectedTint{ 0.35f, 0.35f, 0.35f, 1.0f };

		static AssetKey BackgroundKey()
		{
			return MakeAssetKey(AssetType::Texture, kBackgroundPath);
		}

		int m_selected = 0;
		int m_held = 0;
		double m_repeatTimer = 0.0;
		bool m_wasAccept = false;

		AssetManager* m_assets = nullptr;
		std::unique_ptr<QuadRenderer> m_quads;
		ID3D12Device* m_device = nullptr;
		StretchedImage m_background;
	};
}
