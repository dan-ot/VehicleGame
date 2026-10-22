//
// PlayMode.h
//

#pragma once

#include "../Scene/Scene.h"
#include "../State/EngineMode.h"
#include "../State/ModeHandoff.h"

#include <string>
#include <variant>

namespace Engine
{
	class PlayMode final : public IEngineMode
	{
	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			manifest.Add(AssetType::Model, L"Assets\\vehicle.glb");
			manifest.Add(AssetType::Sound, L"Assets\\engine.wav");
		}

		void OnEnter() override
		{
			if (const auto handoff = TakeHandoff())
			{
				if (const auto* play = std::get_if<PlayHandoff>(&handoff->payload))
				{
					m_levelName = play->levelName;
				}
			}
		}

		void Update(const ModeTick& tick, const CommandFrame& commands) override
		{
			if (commands[Command::System_Abort].greatest.valid)
			{
				RequestTransition(EngineState::Menu);
				return;
			}

			m_throttle = Strength(commands[Command::Movement_Up]);
			m_brake = Strength(commands[Command::Movement_Down]);
			m_scene.Update(tick.modeDt);
		}

	private:
		static float Strength(const CommandValue& value) noexcept
		{
			return value.greatest.valid ? value.greatest.strength : 0.0f;
		}

		Scene m_scene;
		std::wstring m_levelName;
		float m_throttle = 0.0f;
		float m_brake = 0.0f;
	};
}
