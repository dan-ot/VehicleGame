//
// WavLoader.cpp
//

#include "pch.h"
#include "WavLoader.h"

#include <cstring>
#include <fstream>
#include <iterator>

namespace Engine
{
	namespace
	{
		uint32_t ReadU32(const uint8_t* p) noexcept
		{
			return static_cast<uint32_t>(p[0]) | (static_cast<uint32_t>(p[1]) << 8) |
				   (static_cast<uint32_t>(p[2]) << 16) | (static_cast<uint32_t>(p[3]) << 24);
		}

		uint16_t ReadU16(const uint8_t* p) noexcept
		{
			return static_cast<uint16_t>(p[0] | (p[1] << 8));
		}
	}

	bool LoadWav(const std::wstring& path, DecodedSound& sound)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return false;
		}

		std::vector<uint8_t> bytes((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());
		if (bytes.size() < 12 || std::memcmp(bytes.data(), "RIFF", 4) != 0 || std::memcmp(bytes.data() + 8, "WAVE", 4) != 0)
		{
			return false;
		}

		bool haveFormat = false;
		size_t offset = 12;
		while (offset + 8 <= bytes.size())
		{
			const uint8_t* chunk = bytes.data() + offset;
			const uint32_t chunkSize = ReadU32(chunk + 4);
			const size_t bodyStart = offset + 8;
			const size_t available = bytes.size() - bodyStart;
			const size_t bodySize = chunkSize > available ? available : chunkSize;

			if (std::memcmp(chunk, "fmt ", 4) == 0 && bodySize >= 16)
			{
				const uint8_t* fmt = bytes.data() + bodyStart;
				const uint16_t formatTag = ReadU16(fmt);
				sound.channels = ReadU16(fmt + 2);
				sound.sampleRate = ReadU32(fmt + 4);
				const uint16_t bitsPerSample = ReadU16(fmt + 14);
				if (formatTag != 1 || bitsPerSample != 16 || sound.channels == 0)
				{
					return false;
				}
				haveFormat = true;
			}
			else if (std::memcmp(chunk, "data", 4) == 0)
			{
				if (!haveFormat)
				{
					return false;
				}

				const size_t sampleCount = bodySize / sizeof(int16_t);
				sound.samples.resize(sampleCount);
				for (size_t i = 0; i < sampleCount; ++i)
				{
					sound.samples[i] = static_cast<int16_t>(ReadU16(bytes.data() + bodyStart + i * 2));
				}
				return true;
			}

			offset = bodyStart + chunkSize + (chunkSize & 1);
		}

		return false;
	}
}
