//
// AssetManager.h - Engine-level owner of asset entries shared across Core Mode kits
//

#pragma once

#include "AssetKit.h"
#include "FontData.h"
#include "GpuContext.h"

#include <atomic>
#include <cstdint>
#include <memory>
#include <unordered_map>
#include <unordered_set>
#include <vector>

struct ID3D12GraphicsCommandList;

namespace Engine
{
	class TextureResource;
	class FontResource;
	class MeshResource;
	class SoundResource;
	class MusicResource;
	class ShaderResource;

	// GPU-side representation of one asset.
	// GPU objects; they never retain decoded CPU copies, so Rebuild reads the source again.
	// Rebuild runs on the upload thread and records its copies onto the given command list.
	class IAssetResource
	{
	public:
		virtual ~IAssetResource() = default;

		virtual void Release() noexcept = 0;
		virtual bool Rebuild(GpuContext& resources, ID3D12GraphicsCommandList* commandList) = 0;
		virtual bool Load() { return false; }
	};

	class AssetManager
	{
	public:
		AssetManager();
		~AssetManager();

		AssetManager(const AssetManager&) = delete;
		AssetManager& operator=(const AssetManager&) = delete;

		// Replaces the kit's manifest with the given keys. Keys shared with another kit are retained
		// and keys no longer declared are ejected; an entry is evicted only when no requester remains.
		void Declare(AssetKit& kit, const AssetManifest& manifest, GpuContext* resources = nullptr);

		// Adds a reference from the kit to the asset, keyed by (key, requester). When resources is
		// provided, a newly resident entry is built through AssetLoader.
		void Request(AssetKit& kit, const AssetKey& key, GpuContext* resources = nullptr);

		// Removes the kit's reference to the asset and evicts the entry when it has no requesters.
		void Eject(AssetKit& kit, const AssetKey& key);

		// Releases every reference held by the kit.
		void Release(AssetKit& kit);

		// Supplies the GPU context used to build GPU assets declared before a device exists or without one.
		// Builds every resident GPU entry that has no resource yet.
		void BindGpu(GpuContext& gpu);

		// Attaches GPU builds whose upload has completed. Non-blocking; call from the main thread.
		void Pump();

		// True when a build for the key failed. Cleared when the key is requested again.
		bool HasFailed(const AssetKey& key) const;

		// Associates a GPU resource with a resident entry; the entry takes ownership.
		void Attach(const AssetKey& key, std::unique_ptr<IAssetResource> resource);

		// Attaches a worker-decoded resource only when the key is already requested and not yet built.
		// Returns false, and discards nothing, when the key is not resident or already has a resource.
		bool AttachDecoded(const AssetKey& key, std::unique_ptr<IAssetResource>& resource);

		const TextureResource* FindTexture(const AssetKey& key) const;
		const FontResource* FindFont(const AssetKey& key) const;
		const MeshResource* FindModel(const AssetKey& key) const;
		const SoundResource* FindSound(const AssetKey& key) const;
		const MusicResource* FindMusic(const AssetKey& key) const;
		const ShaderResource* FindShader(const AssetKey& key) const;

		bool IsResident(const AssetKey& key) const;
		bool IsBuilt(const AssetKey& key) const;
		size_t RequesterCount(const AssetKey& key) const;
		size_t EntryCount() const noexcept { return m_entries.size(); }

		// Releases all GPU objects but keeps entries and requester links.
		void OnDeviceLost() noexcept;

		// Queues entries without GPU resources for rebuild. Keys held by preferred are queued first.
		void OnDeviceRestored(const AssetKit* preferred);

		// Queues rebuilds for restored resources until budgetSeconds elapse, then pumps completions.
		// Returns true when the queue is empty and no builds are in flight.
		bool RestoreWithinBudget(GpuContext& resources, double budgetSeconds);

	private:
		struct Entry
		{
			std::unordered_set<RequesterId> requesters;
			std::unique_ptr<IAssetResource> resource;
			bool pending = false;
		};

		struct BuildStatus
		{
			std::atomic<bool> done{ false };
			std::atomic<bool> ok{ false };
		};

		struct PendingBuild
		{
			AssetKey key;
			std::shared_ptr<std::unique_ptr<IAssetResource>> holder;
			std::shared_ptr<BuildStatus> status;
			uint64_t token = 0;
			bool discard = false;
		};

		void BuildGpu(GpuContext& gpu, const AssetKey& key, Entry& entry);
		void DiscardPending(const AssetKey& key) noexcept;

		std::unordered_map<AssetKey, Entry> m_entries;
		std::vector<AssetKey> m_restoreQueue;
		size_t m_restoreCursor = 0;
		std::vector<PendingBuild> m_pending;
		std::unordered_set<AssetKey> m_failed;
		GpuContext* m_gpu = nullptr;
		IAssetResource* Find(const AssetKey& key) const;
		AssetUploader& Uploader() noexcept;
		std::unique_ptr<AssetUploader> m_uploader;
	};
}
