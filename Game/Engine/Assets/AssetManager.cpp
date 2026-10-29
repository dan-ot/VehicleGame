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
#include "ShaderResource.h"
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

		if (entry.resource || entry.pending)
		{
			return;
		}

		if (key.type == AssetType::Sound || key.type == AssetType::Music || key.type == AssetType::Shader)
		{
			entry.resource = AssetLoader::CreateCpu(key);
		}
		else if (resources)
		{
			m_failed.erase(key);
			BuildGpu(*resources, key, entry);
		}
	}

	void AssetManager::BindGpu(GpuContext& gpu)
	{
		m_gpu = &gpu;
		for (auto& pair : m_entries)
		{
			const AssetKey& key = pair.first;
			Entry& entry = pair.second;
			if (!entry.resource && !entry.pending && key.type != AssetType::Sound && key.type != AssetType::Music && key.type != AssetType::Shader && m_restoreQueue.empty())
			{
				BuildGpu(gpu, key, entry);
			}
		}
	}

	void AssetManager::BuildGpu(GpuContext& gpu, const AssetKey& key, Entry& entry)
	{
		if (!Uploader().IsStarted())
		{
			Uploader().Start(gpu.GetD3DDevice());
		}

		// Parsing and staging happen on the upload thread; the resource is retained here until it is attached.
		auto status = std::make_shared<BuildStatus>();
		auto holder = std::make_shared<std::unique_ptr<IAssetResource>>();
		const AssetKey keyCopy = key;
		const uint64_t token = Uploader().Enqueue([&gpu, keyCopy, status, holder](ID3D12GraphicsCommandList* commandList) {
			try
			{
				*holder = AssetLoader::CreateAndBuild(keyCopy, gpu, commandList);
			}
			catch (...)
			{
				holder->reset();
			}
			status->ok = *holder != nullptr;
			status->done = true;
		});

		entry.pending = true;
		m_pending.push_back(PendingBuild{ key, holder, status, token, false });
	}

	void AssetManager::Pump()
	{
		for (auto it = m_pending.begin(); it != m_pending.end();)
		{
			if (!it->status->done)
			{
				++it;
				continue;
			}

			const auto entryIt = m_entries.find(it->key);
			if (entryIt != m_entries.end())
			{
				entryIt->second.pending = false;
			}

			if (!it->discard)
			{
				if (it->status->ok && *it->holder)
				{
					if (entryIt != m_entries.end())
					{
						entryIt->second.resource = std::move(*it->holder);
					}
				}
				else
				{
					m_failed.insert(it->key);
				}
			}

			it = m_pending.erase(it);
		}
	}

	bool AssetManager::HasFailed(const AssetKey& key) const
	{
		return m_failed.find(key) != m_failed.end();
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
			DiscardPending(key);
			m_entries.erase(it);
		}
	}

	void AssetManager::DiscardPending(const AssetKey& key) noexcept
	{
		for (PendingBuild& build : m_pending)
		{
			if (build.key == key)
			{
				build.discard = true;
			}
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

	bool AssetManager::AttachDecoded(const AssetKey& key, std::unique_ptr<IAssetResource>& resource)
	{
		const auto it = m_entries.find(key);
		if (it == m_entries.end() || it->second.resource || !resource)
		{
			return false;
		}
		it->second.resource = std::move(resource);
		return true;
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

	const ShaderResource* AssetManager::FindShader(const AssetKey& key) const
	{
		return key.type == AssetType::Shader ? dynamic_cast<const ShaderResource*>(Find(key)) : nullptr;
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

		for (PendingBuild& build : m_pending)
		{
			build.discard = true;
		}

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
			if (pair.first.type != AssetType::Sound && pair.first.type != AssetType::Music && pair.first.type != AssetType::Shader && queued.insert(pair.first).second)
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

			const AssetKey key = m_restoreQueue[m_restoreCursor];
			const auto it = m_entries.find(key);
			if (it != m_entries.end() && !it->second.pending)
			{
				it->second.resource.reset();
				BuildGpu(resources, key, it->second);
			}
			++m_restoreCursor;
		}

		m_restoreQueue.clear();
		m_restoreCursor = 0;
		return true;
	}
}
