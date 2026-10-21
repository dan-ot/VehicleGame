//
// LoadingMode.h
//

#pragma once

#include "../State/EngineMode.h"

namespace Engine
{
	class LoadingMode final : public IEngineMode
	{
	protected:
		void DeclareAssets(AssetManifest& manifest) override
		{
			manifest.Add(AssetType::Texture, L"Assets\\loading_splash.tga");
		}

		void Update(const ModeTick&, const CommandFrame&) override
		{
			// TODO: Replace the fixed timer with an asset-ready signal from the Loading kit.
			if (Elapsed() >= kLoadSeconds)
			{
				RequestTransition(EngineState::Menu);
			}
		}

	private:
		static constexpr double kLoadSeconds = 2.0;
	};
}
