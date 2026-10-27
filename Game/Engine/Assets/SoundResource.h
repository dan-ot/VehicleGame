//
// SoundResource.h - CPU-resident sound asset backed by a 16-bit PCM WAV source file
//

#pragma once

#include "AssetManager.h"
#include "WavLoader.h"

#include <string>

namespace Engine
{
	// Sound is consumed by the CPU audio system, so decoded PCM stays resident in memory.
	// Device loss does not affect it; Release and Rebuild are no-ops.
	class SoundResource final : public IAssetResource
	{
	public:
		explicit SoundResource(std::wstring sourcePath);

		void Release() noexcept override {}
		bool Rebuild(DX::DeviceResources& resources, ID3D12GraphicsCommandList* commandList) override;
		bool Load() override;

		const DecodedSound& Sound() const noexcept { return m_sound; }
		bool IsLoaded() const noexcept { return m_loaded; }

	private:
		std::wstring m_sourcePath;
		DecodedSound m_sound;
		bool m_loaded = false;
	};
}
