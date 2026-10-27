//
// MixerTypes.h - DX- and XAudio2-free types shared by the Mixer and its callers
//

#pragma once

#include <cstddef>
#include <cstdint>

namespace Engine
{
	enum class SoundChannel : uint8_t
	{
		Music,
		System,
		Gameplay,
		Background,
		Other,
	};

	inline constexpr size_t kSoundChannelCount = 5;

	struct SoundParams
	{
		float volume = 1.0f;
		float pitch = 1.0f;
	};

	struct ChannelSettings
	{
		float volume = 1.0f;
		bool muted = false;
	};
}
