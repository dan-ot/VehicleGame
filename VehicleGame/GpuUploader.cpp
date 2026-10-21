//
// GpuUploader.cpp
//

#include "pch.h"
#include "GpuUploader.h"

namespace DX
{
	namespace
	{
		std::vector<uint8_t> PadRows(const uint8_t* data, UINT height, UINT rowPitch, UINT alignedPitch)
		{
			std::vector<uint8_t> padded(static_cast<size_t>(alignedPitch) * height, 0);
			for (UINT row = 0; row < height; ++row)
			{
				std::memcpy(padded.data() + static_cast<size_t>(alignedPitch) * row,
							data + static_cast<size_t>(rowPitch) * row, rowPitch);
			}
			return padded;
		}
	}

	void GpuUploader::Create(ID3D12Device* device, ID3D12CommandQueue* queue, size_t stagingBytes)
	{
		m_device = device;
		m_queue = queue;

		ThrowIfFailed(device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,
			IID_PPV_ARGS(m_allocator.ReleaseAndGetAddressOf())));
		ThrowIfFailed(device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_allocator.Get(),
			nullptr, IID_PPV_ARGS(m_list.ReleaseAndGetAddressOf())));
		ThrowIfFailed(m_list->Close());

		ThrowIfFailed(device->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(m_fence.ReleaseAndGetAddressOf())));
		m_event.Attach(CreateEventEx(nullptr, nullptr, 0, EVENT_ALL_ACCESS));
		if (!m_event.IsValid())
		{
			throw std::runtime_error("GpuUploader: failed to create fence event.");
		}

		m_staging.Create(device, stagingBytes);
	}

	void GpuUploader::CopyBufferAndWait(ID3D12Resource* dest, const void* data, size_t sizeBytes)
	{
		m_staging.Reset();
		size_t offset = 0;
		if (!m_staging.Write(data, sizeBytes, 1, offset))
		{
			throw std::runtime_error("GpuUploader: staging buffer too small for buffer upload.");
		}

		ThrowIfFailed(m_allocator->Reset());
		ThrowIfFailed(m_list->Reset(m_allocator.Get(), nullptr));
		m_list->CopyBufferRegion(dest, 0, m_staging.Get(), offset, sizeBytes);
		Submit();
	}

	void GpuUploader::CopyTextureAndWait(ID3D12Resource* dest, const void* data, UINT width, UINT height,
										 UINT rowPitchBytes, DXGI_FORMAT format)
	{
		const UINT alignedPitch = (rowPitchBytes + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) /
								  D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
		const size_t totalBytes = static_cast<size_t>(alignedPitch) * height;

		m_staging.Reset();
		size_t offset = 0;
		bool written = true;
		if (alignedPitch == rowPitchBytes)
		{
			written = m_staging.Write(data, totalBytes, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, offset);
		}
		else
		{
			const std::vector<uint8_t> padded = PadRows(static_cast<const uint8_t*>(data), height, rowPitchBytes, alignedPitch);
			written = m_staging.Write(padded.data(), padded.size(), D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, offset);
		}
		if (!written)
		{
			throw std::runtime_error("GpuUploader: staging buffer too small for texture upload.");
		}

		D3D12_TEXTURE_COPY_LOCATION src = {};
		src.pResource = m_staging.Get();
		src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
		src.PlacedFootprint.Offset = offset;
		src.PlacedFootprint.Footprint.Format = format;
		src.PlacedFootprint.Footprint.Width = width;
		src.PlacedFootprint.Footprint.Height = height;
		src.PlacedFootprint.Footprint.Depth = 1;
		src.PlacedFootprint.Footprint.RowPitch = alignedPitch;

		D3D12_TEXTURE_COPY_LOCATION dst = {};
		dst.pResource = dest;
		dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
		dst.SubresourceIndex = 0;

		ThrowIfFailed(m_allocator->Reset());
		ThrowIfFailed(m_list->Reset(m_allocator.Get(), nullptr));
		m_list->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);
		Submit();
	}

	void GpuUploader::Submit()
	{
		ThrowIfFailed(m_list->Close());
		ID3D12CommandList* lists[] = { m_list.Get() };
		m_queue->ExecuteCommandLists(1, lists);
		WaitForUpload();
	}

	void GpuUploader::WaitForUpload()
	{
		const UINT64 value = ++m_fenceValue;
		ThrowIfFailed(m_queue->Signal(m_fence.Get(), value));
		if (m_fence->GetCompletedValue() < value)
		{
			ThrowIfFailed(m_fence->SetEventOnCompletion(value, m_event.Get()));
			WaitForSingleObjectEx(m_event.Get(), INFINITE, FALSE);
		}
	}
}
