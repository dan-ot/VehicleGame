//
// WavLoader.h - Hand-parsed 16-bit PCM RIFF/WAVE reader
//

#pragma once

#include <cstdint>
#include <fstream>
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

	// Streams 16-bit PCM frames from a WAV file without decoding the whole data chunk.
	class WavStreamReader
	{
	public:
		bool Open(const std::wstring& path);
		// Reads up to sampleCount int16 samples into out. Returns the number read; 0 at end of data.
		size_t Read(int16_t* out, size_t sampleCount);
		void Rewind();

		uint16_t Channels() const noexcept { return m_channels; }
		uint32_t SampleRate() const noexcept { return m_sampleRate; }

	private:
		std::ifstream m_file;
		std::streamoff m_dataStart = 0;
		std::streamoff m_dataEnd = 0;
		uint16_t m_channels = 0;
		uint32_t m_sampleRate = 0;
	};
}
