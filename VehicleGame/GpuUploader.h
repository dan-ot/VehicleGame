//
// GpuUploader.h - Records and synchronously submits copies from upload buffers to GPU resources
//

#pragma once

#include "UploadBuffer.h"

namespace DX
{
	class GpuUploader
	{
	public:
		GpuUploader() noexcept = default;

		GpuUploader(const GpuUploader&) = delete;
		GpuUploader& operator=(const GpuUploader&) = delete;

		void Create(ID3D12Device* device, ID3D12CommandQueue* queue, size_t stagingBytes);

		// Copies sizeBytes from the staging buffer into dest and waits for the GPU to finish.
		// The staging buffer is reset at the start of each call.
		void CopyBufferAndWait(ID3D12Resource* dest, const void* data, size_t sizeBytes);

		// Copies pixel rows into a texture subresource and waits for the GPU to finish.
		void CopyTextureAndWait(ID3D12Resource* dest, const void* data, UINT width, UINT height,
								UINT rowPitchBytes, DXGI_FORMAT format);

	private:
		void Submit();
		void WaitForUpload();

		ID3D12Device* m_device = nullptr;
		ID3D12CommandQueue* m_queue = nullptr;
		Microsoft::WRL::ComPtr<ID3D12CommandAllocator> m_allocator;
		Microsoft::WRL::ComPtr<ID3D12GraphicsCommandList> m_list;
		Microsoft::WRL::ComPtr<ID3D12Fence> m_fence;
		Microsoft::WRL::Wrappers::Event m_event;
		UINT64 m_fenceValue = 0;
		UploadBuffer m_staging;
	};
}
