//
// UploadBuffer.h - CPU-writable D3D12 upload heap used to stage asset data for GPU copies
//

#pragma once

#include <wrl/client.h>
#include <wrl/event.h>
#include <directx/d3d12.h>
#include <directx/d3dx12.h>

#include <cstring>

namespace DX
{
	class UploadBuffer
	{
	public:
		UploadBuffer() noexcept = default;

		UploadBuffer(const UploadBuffer&) = delete;
		UploadBuffer& operator=(const UploadBuffer&) = delete;

		void Create(ID3D12Device* device, size_t capacityBytes)
		{
			const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_UPLOAD);
			const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(capacityBytes);

			ThrowIfFailed(device->CreateCommittedResource(
				&heapProps,
				D3D12_HEAP_FLAG_NONE,
				&desc,
				D3D12_RESOURCE_STATE_GENERIC_READ,
				nullptr,
				IID_PPV_ARGS(m_resource.ReleaseAndGetAddressOf())));

			ThrowIfFailed(m_resource->Map(0, nullptr, &m_mapped));
			m_capacity = capacityBytes;
			m_offset = 0;
		}

		// Reserves aligned space and copies the data into it. Returns the offset in the buffer,
		// or false when the buffer lacks room for the write.
		bool Write(const void* data, size_t sizeBytes, size_t alignment, size_t& outOffset) noexcept
		{
			const size_t aligned = (m_offset + alignment - 1) / alignment * alignment;
			if (aligned + sizeBytes > m_capacity)
			{
				return false;
			}

			std::memcpy(static_cast<uint8_t*>(m_mapped) + aligned, data, sizeBytes);
			outOffset = aligned;
			m_offset = aligned + sizeBytes;
			return true;
		}

		// Discards all staged data. Only call after the GPU has finished reading the buffer.
		void Reset() noexcept
		{
			m_offset = 0;
		}

		ID3D12Resource* Get() const noexcept { return m_resource.Get(); }
		size_t Capacity() const noexcept { return m_capacity; }
		size_t Used() const noexcept { return m_offset; }

	private:
		Microsoft::WRL::ComPtr<ID3D12Resource> m_resource;
		void* m_mapped = nullptr;
		size_t m_capacity = 0;
		size_t m_offset = 0;
	};
}
