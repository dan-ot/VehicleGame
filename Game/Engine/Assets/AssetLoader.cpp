//
// AssetLoader.cpp
//

#include "pch.h"
#include "AssetLoader.h"
#include "../../DeviceResources.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "SoundResource.h"
#include "FontResource.h"

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
			case AssetType::Font:
				return std::make_unique<FontResource>(key.path);
			default:
				return nullptr;
			}
		}
		catch (...)
		{
			return nullptr;
		}
	}

	std::unique_ptr<IAssetResource> AssetLoader::CreateAndBuild(const AssetKey& key, GpuContext& resources, AssetUploader& uploader) noexcept
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
			built = resource->Rebuild(resources, uploader);
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
