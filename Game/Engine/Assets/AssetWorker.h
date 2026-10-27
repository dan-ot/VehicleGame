//
// AssetWorker.h - Single-batch background decode for CPU-only asset work (no D3D objects)
//

#pragma once

#include "AssetKey.h"
#include "AssetManager.h"

#include <atomic>
#include <cstddef>
#include <functional>
#include <memory>
#include <mutex>
#include <thread>
#include <utility>
#include <vector>

namespace Engine
{
	class AssetWorker
	{
	public:
		using DecodeJob = std::function<std::unique_ptr<IAssetResource>()>;

		struct Result
		{
			AssetKey key;
			std::unique_ptr<IAssetResource> resource;
		};

		AssetWorker() = default;
		~AssetWorker() { Join(); }

		AssetWorker(const AssetWorker&) = delete;
		AssetWorker& operator=(const AssetWorker&) = delete;

		// Starts one batch. Returns false, and changes nothing, if a batch is still active.
		bool Begin(std::vector<std::pair<AssetKey, DecodeJob>> jobs)
		{
			if (Busy())
			{
				return false;
			}
			Join();

			m_total = jobs.size();
			m_decoded.store(0);
			m_finished.store(false);
			m_thread = std::thread([this, jobs = std::move(jobs)]() mutable { Run(std::move(jobs)); });
			return true;
		}

		bool Busy() const noexcept { return m_thread.joinable() && !m_finished.load(); }

		// Fraction of the active batch that has finished decoding, in [0, 1].
		double Progress() const noexcept
		{
			return m_total == 0 ? 1.0 : static_cast<double>(m_decoded.load()) / static_cast<double>(m_total);
		}

		// Moves out every decode completed so far. Results are not GPU-built; the main thread attaches them.
		std::vector<Result> TakeCompleted()
		{
			std::lock_guard lock(m_mutex);
			return std::exchange(m_completed, {});
		}

		// True once every job in the batch has run and the thread has ended its work.
		bool Finished() const noexcept { return m_finished.load(); }

		// Waits for the thread to exit and clears the batch state. Call after Finished() or before shutdown.
		void Join() noexcept
		{
			if (m_thread.joinable())
			{
				m_thread.join();
			}
		}

	private:
		void Run(std::vector<std::pair<AssetKey, DecodeJob>> jobs)
		{
			for (auto& [key, decode] : jobs)
			{
				std::unique_ptr<IAssetResource> resource = decode();
				{
					std::lock_guard lock(m_mutex);
					m_completed.push_back(Result{ key, std::move(resource) });
				}
				m_decoded.fetch_add(1);
			}
			m_finished.store(true);
		}

		std::thread m_thread;
		mutable std::mutex m_mutex;
		std::vector<Result> m_completed;
		std::atomic<std::size_t> m_decoded{ 0 };
		std::size_t m_total = 0;
		std::atomic<bool> m_finished{ false };
	};
}
