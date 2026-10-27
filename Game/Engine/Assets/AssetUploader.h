//
// AssetUploader.h - Background D3D12 copy queue for asset uploads
//

#pragma once

#include "../../DeviceResources.h"

#include <condition_variable>
#include <cstdint>
#include <deque>
#include <functional>
#include <mutex>
#include <thread>

namespace Engine
{
	class AssetUploader
	{
	public:
		using UploadJob = std::function<void(ID3D12GraphicsCommandList* commandList)>;

		AssetUploader() = default;
		~AssetUploader();

		AssetUploader(const AssetUploader&) = delete;
		AssetUploader& operator=(const AssetUploader&) = delete;

		void Start(ID3D12Device* device);
		void Stop() noexcept;
		bool IsStarted() const noexcept;

		// Returns a token that IsComplete() accepts once the job's GPU work has finished.
		uint64_t Enqueue(UploadJob job);
		bool IsComplete(uint64_t token) const noexcept;
		bool IsIdle() const noexcept;

	private:
		struct PendingJob
		{
			UploadJob record;
			std::function<void()> onComplete;
			uint64_t token = 0;
		};

		void Run();
		void WaitForFence(uint64_t value) noexcept;

		ID3D12Device* m_device = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandQueue> m_copyQueue;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_commandList;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		Microsoft::WRL::Wrappers::Event m_fenceEvent;
		uint64_t m_issuedValue = 0;

		std::thread m_thread;
		mutable std::mutex m_mutex;
		std::condition_variable m_wake;
		std::deque<PendingJob> m_jobs;
		bool m_running = false;
		bool m_busy = false;
	};
}
