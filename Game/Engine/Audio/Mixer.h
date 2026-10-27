//
// Mixer.h - XAudio2 sound mixer with fixed channels and a fixed voice pool (public, XAudio2-free)
//

#pragma once

#include "MixerTypes.h"
#include "../Assets/AssetKey.h"

#include <memory>
#include <vector>

namespace Engine
{
	class AssetManager;
	class AssetKit;

	class Mixer
	{
	public:
		explicit Mixer(AssetManager& assets);
		~Mixer();

		Mixer(const Mixer&) = delete;
		Mixer& operator=(const Mixer&) = delete;

		// Creates the XAudio2 engine and the voice pool. Returns false when audio is unavailable.
		bool Initialize();
		void Shutdown() noexcept;

		// Music, System and Background persist across mode changes; Gameplay and Other stop on mode exit.
		void OnModeExit();

		void PlaySound(SoundChannel channel, const AssetKey& key, const SoundParams& params = {});
		void PlayMusic(const AssetKey& key, const SoundParams& params = {});
		void StopMusic();

		// Identical sets are no-ops, an empty set silences Background, and a new set replaces the previous one.
		void SetBackgroundSet(const std::vector<AssetKey>& keys);

		void SetChannelSettings(SoundChannel channel, const ChannelSettings& settings);
		ChannelSettings GetChannelSettings(SoundChannel channel) const;

		// Keeps the streaming music and background voices fed. Call once per frame.
		void Update();

		AssetKit& PersistentKit() noexcept;

	private:
		class Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
