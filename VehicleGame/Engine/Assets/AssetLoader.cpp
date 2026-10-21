//
// AssetLoader.cpp
//

#include "pch.h"
#include "AssetLoader.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "SoundResource.h"

namespace Engine
{
	std::unique_ptr<IAssetResource> AssetLoader::Create(const AssetKey& key) noexcept
	{
		try
		{
			switch (key.type)
			{
			case AssetType::Texture:
				return std::make_unique<TextureResource>(key.path);
			case AssetType::Model:
				return std::make_unique<MeshResource>(key.path);
			case AssetType::Sound:
				return std::make_unique<SoundResource>(key.path);
			default:
				return nullptr;
			}
		}
		catch (...)
		{
			return nullptr;
		}
	}

	std::unique_ptr<IAssetResource> AssetLoader::CreateAndBuild(const AssetKey& key, DX::DeviceResources& resources) noexcept
	{
		auto resource = Create(key);
		if (!resource)
		{
			OutputDebugStringW((L"AssetLoader: unsupported asset type for " + key.path + L"\n").c_str());
			return nullptr;
		}

		bool built = false;
		try
		{
			built = resource->Rebuild(resources);
		}
		catch (...)
		{
			built = false;
		}

		if (!built)
		{
			OutputDebugStringW((L"AssetLoader: failed to load " + key.path + L"\n").c_str());
			return nullptr;
		}
		return resource;
	}
}
