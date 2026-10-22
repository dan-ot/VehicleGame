//
// WavLoader.h - Hand-parsed 16-bit PCM RIFF/WAVE reader
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Engine
{
	struct DecodedSound
	{
		uint16_t channels = 0;
		uint32_t sampleRate = 0;
		std::vector<int16_t> samples;
	};

	// Reads a 16-bit PCM WAV file by walking RIFF chunks for 'fmt ' and 'data'.
	bool LoadWav(const std::wstring& path, DecodedSound& sound);
}
