//
// AssetLoader.h - Maps asset keys to their GPU or CPU resource implementations
//

#pragma once

#include "AssetKey.h"
#include "AssetManager.h"

#include <memory>

namespace Engine
{
	class AssetLoader
	{
	public:
		// Creates the resource implementation for the key's type. Returns null for unsupported types.
		static std::unique_ptr<IAssetResource> Create(const AssetKey& key) noexcept;

		// Creates and rebuilds the resource. Returns null and logs when the source cannot be loaded.
		static std::unique_ptr<IAssetResource> CreateAndBuild(const AssetKey& key, DX::DeviceResources& resources) noexcept;
	};
}
