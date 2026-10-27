//
// AssetUploader.cpp - Background D3D12 copy queue for asset uploads
//

#include "pch.h"
#include "AssetUploader.h"

namespace Engine
{
	AssetUploader::~AssetUploader()
	{
		Stop();
	}

	void AssetUploader::Start(ID3D12Device* device)
	{
		Stop();

		m_device = device;

		D3D12_COMMAND_QUEUE_DESC queueDesc = {};
		queueDesc.Type = D3D12_COMMAND_LIST_TYPE_COPY;
		queueDesc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
		DX::ThrowIfFailed(m_device->CreateCommandQueue(&queueDesc, IID_PPV_ARGS(m_copyQueue.ReleaseAndGetAddressOf())));
		DX::ThrowIfFailed(m_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(m_allocator.ReleaseAndGetAddressOf())));
		DX::ThrowIfFailed(m_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, m_allocator.Get(), nullptr, IID_PPV_ARGS(m_commandList.ReleaseAndGetAddressOf())));
		m_commandList->Close();

		m_fence.Reset();
		DX::ThrowIfFailed(m_device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
		m_issuedValue = 0;
		m_fenceEvent.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_MODIFY_STATE | SYNCHRONIZE));
		if (!m_fenceEvent.IsValid())
		{
			throw std::runtime_error("AssetUploader: failed to create fence event.");
		}

		{
			std::lock_guard lock(m_mutex);
			m_running = true;
		}
		m_thread = std::thread([this] { Run(); });
	}

	void AssetUploader::Stop() noexcept
	{
		{
			std::lock_guard lock(m_mutex);
			if (!m_running)
			{
				return;
			}
			m_running = false;
		}
		m_wake.notify_all();
		if (m_thread.joinable())
		{
			m_thread.join();
		}
		m_copyQueue.Reset();
		m_commandList.Reset();
		m_allocator.Reset();
		m_fence.Reset();
	}

	bool AssetUploader::IsStarted() const noexcept
	{
		std::lock_guard lock(m_mutex);
		return m_running;
	}

	uint64_t AssetUploader::Enqueue(UploadJob job)
	{
		uint64_t token = 0;
		{
			std::lock_guard lock(m_mutex);
			token = ++m_issuedValue;
			m_jobs.push_back(PendingJob{ std::move(job), nullptr, token });
		}
		m_wake.notify_one();
		return token;
	}

	bool AssetUploader::IsComplete(uint64_t token) const noexcept
	{
		std::lock_guard lock(m_mutex);
		return m_fence && m_fence->GetCompletedValue() >= token;
	}

	bool AssetUploader::IsIdle() const noexcept
	{
		std::lock_guard lock(m_mutex);
		return m_jobs.empty() && !m_busy;
	}

	void AssetUploader::Run()
	{
		for (;;)
		{
			PendingJob pending;
			{
				std::unique_lock lock(m_mutex);
				m_wake.wait(lock, [this] { return !m_jobs.empty() || !m_running; });
				if (m_jobs.empty())
				{
					return;
				}
				pending = std::move(m_jobs.front());
				m_jobs.pop_front();
				m_busy = true;
			}

			m_allocator->Reset();
			m_commandList->Reset(m_allocator.Get(), nullptr);
			try
			{
				pending.record(m_commandList.Get());
			}
			catch (...)
			{
				OutputDebugStringW(L"AssetUploader: upload job threw; job skipped.\n");
			}
			m_commandList->Close();

			ID3D12CommandList* lists[] = { m_commandList.Get() };
			m_copyQueue->ExecuteCommandLists(1, lists);
			const uint64_t value = pending.token;
			m_copyQueue->Signal(m_fence.Get(), value);
			WaitForFence(value);

			{
				std::lock_guard lock(m_mutex);
				m_busy = false;
			}
			if (pending.onComplete)
			{
				pending.onComplete();
			}
		}
	}

	void AssetUploader::WaitForFence(uint64_t value) noexcept
	{
		if (m_fence->GetCompletedValue() < value)
		{
			m_fence->SetEventOnCompletion(value, m_fenceEvent.Get());
			WaitForSingleObjectEx(m_fenceEvent.Get(), INFINITE, FALSE);
		}
	}
}
