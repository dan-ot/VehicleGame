//
// AssetLoader.h - Maps asset keys to their GPU or CPU resource implementations
//

#pragma once

#include "AssetKey.h"
#include "AssetManager.h"
#include "AssetUploader.h"

#include <memory>

namespace Engine
{
	class AssetLoader
	{
	public:
		// Creates the resource implementation for the key's type. Returns null for unsupported types.
		static std::unique_ptr<IAssetResource> Create(const AssetKey& key) noexcept;

		// Creates and records the GPU build onto the upload command list. Returns null and logs when the source cannot be loaded.
		static std::unique_ptr<IAssetResource> CreateAndBuild(const AssetKey& key, GpuContext& resources, ID3D12GraphicsCommandList* commandList) noexcept;

		// Creates and loads a CPU-only resource (Sound, Music). Returns null for GPU types or failed loads.
		static std::unique_ptr<IAssetResource> CreateCpu(const AssetKey& key) noexcept;
	};
}
