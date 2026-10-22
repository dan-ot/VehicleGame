//
// AssetManager.h - Engine-level owner of asset entries shared across Core Mode kits
//

#pragma once

#include "../../DeviceResources.h"
#include "AssetKit.h"
#include "AssetUploader.h"

#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

namespace Engine
{
	// GPU-side representation of one asset. Implementations keep only the source path and
	// GPU objects; they never retain decoded CPU copies, so Rebuild reads the source again.
	class IAssetResource
	{
	public:
		virtual ~IAssetResource() = default;

		virtual void Release() noexcept = 0;
		virtual bool Rebuild(DX::DeviceResources& resources, AssetUploader& uploader) = 0;
	};

	class AssetManager
	{
	public:
		AssetManager() = default;

		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

		// Replaces the kit's manifest with the given keys. Keys shared with another kit are retained
		// and keys no longer declared are ejected; an entry is evicted only when no requester remains.
		void Declare(AssetKit& kit, const AssetManifest& manifest, DX::DeviceResources* resources = nullptr);

		// Adds a reference from the kit to the asset, keyed by (key, requester). When resources is
		// provided, a newly resident entry is built through AssetLoader.
		void Request(AssetKit& kit, const AssetKey& key, DX::DeviceResources* resources = nullptr);

		// Removes the kit's reference to the asset and evicts the entry when it has no requesters.
		void Eject(AssetKit& kit, const AssetKey& key);

		// Releases every reference held by the kit.
		void Release(AssetKit& kit);

		// Associates a GPU resource with a resident entry; the entry takes ownership.
		void Attach(const AssetKey& key, std::unique_ptr<IAssetResource> resource);

		IAssetResource* Find(const AssetKey& key) const;

		bool IsResident(const AssetKey& key) const;
		size_t RequesterCount(const AssetKey& key) const;
		size_t EntryCount() const noexcept { return m_entries.size(); }

		// Releases all GPU objects but keeps entries and requester links.
		void OnDeviceLost() noexcept;

		// Queues entries without GPU resources for rebuild. Keys held by preferred are queued first.
		void OnDeviceRestored(const AssetKit* preferred);

		// Rebuilds queued resources until budgetSeconds elapse. Returns true when the queue is empty.
		bool RestoreWithinBudget(DX::DeviceResources& resources, double budgetSeconds);

	private:
		struct Entry
		{
			std::unordered_set<EngineState> requesters;
			std::unique_ptr<IAssetResource> resource;
		};

		std::unordered_map<AssetKey, Entry> m_entries;
		std::vector<AssetKey> m_restoreQueue;
		size_t m_restoreCursor = 0;
		AssetUploader m_uploader;
	};
}
