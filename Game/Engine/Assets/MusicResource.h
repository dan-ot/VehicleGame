//
// MusicResource.h - CPU-side music asset that streams from a 16-bit PCM WAV source file
//

#pragma once

#include "AssetManager.h"
#include "WavLoader.h"

#include <memory>
#include <string>

namespace Engine
{
	// Music is too long to decode fully, so only the path and format are kept resident.
	// Each playback opens its own WavStreamReader through OpenStream.
	class MusicResource final : public IAssetResource
	{
	public:
		explicit MusicResource(std::wstring sourcePath) : m_sourcePath(std::move(sourcePath)) {}

		void Release() noexcept override {}
		bool Rebuild(DX::DeviceResources&, ID3D12GraphicsCommandList*) override { return Load(); }

		bool Load() override
		{
			if (m_loaded)
			{
				return true;
			}

			WavStreamReader probe;
			if (!probe.Open(m_sourcePath))
			{
				return false;
			}

			m_channels = probe.Channels();
			m_sampleRate = probe.SampleRate();
			m_loaded = true;
			return true;
		}

		std::unique_ptr<WavStreamReader> OpenStream() const
		{
			auto reader = std::make_unique<WavStreamReader>();
			if (!reader->Open(m_sourcePath))
			{
				return nullptr;
			}
			return reader;
		}

		const std::wstring& SourcePath() const noexcept { return m_sourcePath; }
		uint16_t Channels() const noexcept { return m_channels; }
		uint32_t SampleRate() const noexcept { return m_sampleRate; }
		bool IsLoaded() const noexcept { return m_loaded; }

	private:
		std::wstring m_sourcePath;
		uint16_t m_channels = 0;
		uint32_t m_sampleRate = 0;
		bool m_loaded = false;
	};
}
