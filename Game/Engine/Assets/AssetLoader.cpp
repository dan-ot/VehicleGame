//
// AssetLoader.cpp
//

#include "pch.h"
#include "AssetLoader.h"
#include "../../DeviceResources.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "SoundResource.h"
#include "MusicResource.h"
#include "FontResource.h"
#include "ShaderResource.h"

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
			case AssetType::Music:
				return std::make_unique<MusicResource>(key.path);
			case AssetType::Font:
				return std::make_unique<FontResource>(key.path);
			case AssetType::Shader:
				return std::make_unique<ShaderResource>(key.path);
			default:
				return nullptr;
			}
		}
		catch (...)
		{
			return nullptr;
		}
	}

	std::unique_ptr<IAssetResource> AssetLoader::CreateAndBuild(const AssetKey& key, GpuContext& resources, ID3D12GraphicsCommandList* commandList) noexcept
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
			built = resource->Rebuild(resources, commandList);
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

	std::unique_ptr<IAssetResource> AssetLoader::CreateCpu(const AssetKey& key) noexcept
	{
		if (key.type != AssetType::Sound && key.type != AssetType::Music && key.type != AssetType::Shader)
		{
			return nullptr;
		}

		auto resource = Create(key);
		if (!resource)
		{
			return nullptr;
		}

		bool built = false;
		try
		{
			built = resource->Load();
		}
		catch (...)
		{
			built = false;
		}

		return built ? std::move(resource) : nullptr;
	}
}
