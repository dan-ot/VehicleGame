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

	bool WavStreamReader::Open(const std::wstring& path)
	{
		m_file.close();
		m_file.clear();
		m_file.open(path, std::ios::binary);
		if (!m_file)
		{
			return false;
		}

		uint8_t header[12];
		m_file.read(reinterpret_cast<char*>(header), sizeof(header));
		if (!m_file || std::memcmp(header, "RIFF", 4) != 0 || std::memcmp(header + 8, "WAVE", 4) != 0)
		{
			return false;
		}

		bool haveFormat = false;
		for (;;)
		{
			uint8_t chunk[8];
			m_file.read(reinterpret_cast<char*>(chunk), sizeof(chunk));
			if (!m_file)
			{
				return false;
			}

			const uint32_t chunkSize = ReadU32(chunk + 4);
			if (std::memcmp(chunk, "fmt ", 4) == 0)
			{
				if (chunkSize < 16)
				{
					return false;
				}

				uint8_t fmt[16];
				m_file.read(reinterpret_cast<char*>(fmt), sizeof(fmt));
				if (!m_file)
				{
					return false;
				}

				const uint16_t formatTag = ReadU16(fmt);
				m_channels = ReadU16(fmt + 2);
				m_sampleRate = ReadU32(fmt + 4);
				const uint16_t bitsPerSample = ReadU16(fmt + 14);
				if (formatTag != 1 || bitsPerSample != 16 || m_channels == 0)
				{
					return false;
				}
				haveFormat = true;

				m_file.seekg(static_cast<std::streamoff>(chunkSize - 16 + (chunkSize & 1)), std::ios::cur);
			}
			else if (std::memcmp(chunk, "data", 4) == 0)
			{
				if (!haveFormat)
				{
					return false;
				}

				m_dataStart = m_file.tellg();
				m_dataEnd = m_dataStart + static_cast<std::streamoff>(chunkSize);
				return true;
			}
			else
			{
				m_file.seekg(static_cast<std::streamoff>(chunkSize + (chunkSize & 1)), std::ios::cur);
			}
		}
	}

	size_t WavStreamReader::Read(int16_t* out, size_t sampleCount)
	{
		if (!m_file.is_open() || out == nullptr)
		{
			return 0;
		}

		const std::streamoff position = m_file.tellg();
		if (position < m_dataStart || position >= m_dataEnd)
		{
			return 0;
		}

		const std::streamoff remainingBytes = m_dataEnd - position;
		const size_t maxSamples = static_cast<size_t>(remainingBytes) / sizeof(int16_t);
		const size_t wanted = sampleCount < maxSamples ? sampleCount : maxSamples;

		std::vector<uint8_t> bytes(wanted * sizeof(int16_t));
		m_file.read(reinterpret_cast<char*>(bytes.data()), static_cast<std::streamsize>(bytes.size()));
		const size_t readBytes = static_cast<size_t>(m_file.gcount());
		m_file.clear();
		const size_t readSamples = readBytes / sizeof(int16_t);
		for (size_t i = 0; i < readSamples; ++i)
		{
			out[i] = static_cast<int16_t>(ReadU16(bytes.data() + i * 2));
		}
		return readSamples;
	}

	void WavStreamReader::Rewind()
	{
		if (m_file.is_open())
		{
			m_file.clear();
			m_file.seekg(m_dataStart);
		}
	}
}
