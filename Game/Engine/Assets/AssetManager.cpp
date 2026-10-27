//
// AssetManager.cpp
//

#include "pch.h"
#include "AssetManager.h"
#include "AssetLoader.h"
#include "AssetUploader.h"
#include "TextureResource.h"
#include "MeshResource.h"
#include "SoundResource.h"
#include "MusicResource.h"
#include "FontResource.h"
#include "../../DeviceResources.h"

#include <chrono>
#include <unordered_set>

namespace Engine
{
	AssetManager::AssetManager() : m_uploader(std::make_unique<AssetUploader>()) {}

	AssetManager::~AssetManager() = default;

	AssetUploader& AssetManager::Uploader() noexcept
	{
		return *m_uploader;
	}

	void AssetManager::Declare(AssetKit& kit, const AssetManifest& manifest, GpuContext* resources)
	{
		std::unordered_set<AssetKey> declared;
		for (const AssetKey& key : manifest.Keys())
		{
			declared.insert(key);
		}

		const std::vector<AssetKey> previouslyHeld(kit.HeldKeys().begin(), kit.HeldKeys().end());
		for (const AssetKey& key : previouslyHeld)
		{
			if (declared.find(key) == declared.end())
			{
				Eject(kit, key);
			}
		}

		kit.Manifest() = manifest;

		for (const AssetKey& key : declared)
		{
			Request(kit, key, resources);
		}
	}

	void AssetManager::Request(AssetKit& kit, const AssetKey& key, GpuContext* resources)
	{
		if (kit.Holds(key))
		{
			return;
		}

		Entry& entry = m_entries[key];
		entry.requesters.insert(kit.Requester());
		kit.MarkHeld(key);

		if (entry.resource)
		{
			return;
		}

		if (key.type == AssetType::Sound || key.type == AssetType::Music)
		{
			entry.resource = AssetLoader::CreateCpu(key);
		}
		else if (resources)
		{
			if (!Uploader().IsStarted())
			{
				Uploader().Start(resources->GetD3DDevice());
			}
			entry.resource = AssetLoader::CreateAndBuild(key, *resources, Uploader());
		}
	}

	void AssetManager::Eject(AssetKit& kit, const AssetKey& key)
	{
		if (!kit.Holds(key))
		{
			return;
		}

		kit.MarkReleased(key);

		const auto it = m_entries.find(key);
		if (it == m_entries.end())
		{
			return;
		}

		it->second.requesters.erase(kit.Requester());
		if (it->second.requesters.empty())
		{
			m_entries.erase(it);
		}
	}

	void AssetManager::Release(AssetKit& kit)
	{
		const std::vector<AssetKey> held(kit.HeldKeys().begin(), kit.HeldKeys().end());
		for (const AssetKey& key : held)
		{
			Eject(kit, key);
		}
	}

	void AssetManager::Attach(const AssetKey& key, std::unique_ptr<IAssetResource> resource)
	{
		const auto it = m_entries.find(key);
		if (it != m_entries.end())
		{
			it->second.resource = std::move(resource);
		}
	}

	IAssetResource* AssetManager::Find(const AssetKey& key) const
	{
		const auto it = m_entries.find(key);
		return it == m_entries.end() ? nullptr : it->second.resource.get();
	}

	const TextureResource* AssetManager::FindTexture(const AssetKey& key) const
	{
		return key.type == AssetType::Texture ? dynamic_cast<const TextureResource*>(Find(key)) : nullptr;
	}

	const FontResource* AssetManager::FindFont(const AssetKey& key) const
	{
		return key.type == AssetType::Font ? dynamic_cast<const FontResource*>(Find(key)) : nullptr;
	}

	const MeshResource* AssetManager::FindModel(const AssetKey& key) const
	{
		return key.type == AssetType::Model ? dynamic_cast<const MeshResource*>(Find(key)) : nullptr;
	}

	const SoundResource* AssetManager::FindSound(const AssetKey& key) const
	{
		return key.type == AssetType::Sound ? dynamic_cast<const SoundResource*>(Find(key)) : nullptr;
	}

	const MusicResource* AssetManager::FindMusic(const AssetKey& key) const
	{
		return key.type == AssetType::Music ? dynamic_cast<const MusicResource*>(Find(key)) : nullptr;
	}

	bool AssetManager::IsResident(const AssetKey& key) const
	{
		return m_entries.find(key) != m_entries.end();
	}

	bool AssetManager::IsBuilt(const AssetKey& key) const
	{
		return Find(key) != nullptr;
	}

	size_t AssetManager::RequesterCount(const AssetKey& key) const
	{
		const auto it = m_entries.find(key);
		return it == m_entries.end() ? 0 : it->second.requesters.size();
	}

	void AssetManager::OnDeviceLost() noexcept
	{
		m_restoreQueue.clear();
		m_restoreCursor = 0;

		for (auto& pair : m_entries)
		{
			if (pair.second.resource)
			{
				pair.second.resource->Release();
			}
		}

		m_uploader->Stop();
	}

	void AssetManager::OnDeviceRestored(const AssetKit* preferred)
	{
		m_restoreQueue.clear();
		m_restoreCursor = 0;

		std::unordered_set<AssetKey> queued;
		if (preferred)
		{
			for (const AssetKey& key : preferred->HeldKeys())
			{
				if (queued.insert(key).second && Find(key))
				{
					m_restoreQueue.push_back(key);
				}
			}
		}

		for (const auto& pair : m_entries)
		{
			if (pair.second.resource && queued.insert(pair.first).second)
			{
				m_restoreQueue.push_back(pair.first);
			}
		}
	}

	bool AssetManager::RestoreWithinBudget(GpuContext& resources, double budgetSeconds)
	{
		const auto start = std::chrono::steady_clock::now();

		if (m_restoreCursor < m_restoreQueue.size() && !Uploader().IsStarted())
		{
			Uploader().Start(resources.GetD3DDevice());
		}

		while (m_restoreCursor < m_restoreQueue.size())
		{
			const double spent = std::chrono::duration<double>(std::chrono::steady_clock::now() - start).count();
			if (spent >= budgetSeconds)
			{
				return false;
			}

			if (IAssetResource* resource = Find(m_restoreQueue[m_restoreCursor]))
			{
				resource->Rebuild(resources, Uploader());
			}
			++m_restoreCursor;
		}

		m_restoreQueue.clear();
		m_restoreCursor = 0;
		return true;
	}
}
