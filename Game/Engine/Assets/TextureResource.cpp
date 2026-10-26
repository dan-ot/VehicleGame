//
// TextureResource.cpp
//

#include "pch.h"
#include "TextureResource.h"
#include "../../DeviceResources.h"
#include "TgaLoader.h"
#include "../../UploadBuffer.h"

#include <cstring>
#include <vector>

namespace Engine
{
	TextureResource::TextureResource(std::wstring sourcePath)
		: m_sourcePath(std::move(sourcePath))
	{
	}

	void TextureResource::Release() noexcept
	{
		m_texture.Reset();
		m_cpuDescriptor = {};
		m_gpuDescriptor = {};
		m_width = 0;
		m_height = 0;
	}

	bool TextureResource::Rebuild(DX::DeviceResources& resources, AssetUploader& uploader)
	{
		Release();

		DecodedImage image;
		if (!LoadTga(m_sourcePath, image))
		{
			return false;
		}

		auto* device = resources.GetD3DDevice();

		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM, image.width, image.height, 1, 1);

		DX::ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_texture.ReleaseAndGetAddressOf())));

		const size_t rowPitch = (static_cast<size_t>(image.width) * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) /
			D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
		const size_t totalBytes = rowPitch * image.height;

		std::vector<uint8_t> padded(totalBytes, 0);
		for (UINT row = 0; row < image.height; ++row)
		{
			std::memcpy(padded.data() + row * rowPitch, image.pixelsRgba8.data() + static_cast<size_t>(row) * image.width * 4,
				static_cast<size_t>(image.width) * 4);
		}

		DX::UploadBuffer staging;
		staging.Create(device, totalBytes + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
		size_t offset = 0;
		if (!staging.Write(padded.data(), totalBytes, D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT, offset))
		{
			return false;
		}

		ID3D12Resource* dest = m_texture.Get();
		ID3D12Resource* source = staging.Get();
		const UINT width = image.width;
		const UINT height = image.height;
		uploader.ExecuteAndWait([dest, source, offset, width, height, rowPitch](ID3D12GraphicsCommandList* commandList) {
			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = source;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint.Offset = offset;
			src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			src.PlacedFootprint.Footprint.Width = width;
			src.PlacedFootprint.Footprint.Height = height;
			src.PlacedFootprint.Footprint.Depth = 1;
			src.PlacedFootprint.Footprint.RowPitch = static_cast<UINT>(rowPitch);

			D3D12_TEXTURE_COPY_LOCATION dst = {};
			dst.pResource = dest;
			dst.Type = D3D12_TEXTURE_COPY_TYPE_SUBRESOURCE_INDEX;
			dst.SubresourceIndex = 0;

			commandList->CopyTextureRegion(&dst, 0, 0, 0, &src, nullptr);

			const CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
				dest, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_PIXEL_SHADER_RESOURCE);
			commandList->ResourceBarrier(1, &barrier);
		});

		const UINT index = resources.AllocateSrvIndex();
		const UINT size = resources.GetSrvDescriptorSize();
		const auto cpuStart = resources.GetSrvDescriptorHeap()->GetCPUDescriptorHandleForHeapStart();
		const auto gpuStart = resources.GetSrvDescriptorHeap()->GetGPUDescriptorHandleForHeapStart();
		m_cpuDescriptor = CD3DX12_CPU_DESCRIPTOR_HANDLE(cpuStart, static_cast<INT>(index), size);
		m_gpuDescriptor = CD3DX12_GPU_DESCRIPTOR_HANDLE(gpuStart, static_cast<INT>(index), size);

		D3D12_SHADER_RESOURCE_VIEW_DESC srvDesc = {};
		srvDesc.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
		srvDesc.ViewDimension = D3D12_SRV_DIMENSION_TEXTURE2D;
		srvDesc.Shader4ComponentMapping = D3D12_DEFAULT_SHADER_4_COMPONENT_MAPPING;
		srvDesc.Texture2D.MipLevels = 1;
		device->CreateShaderResourceView(m_texture.Get(), &srvDesc, m_cpuDescriptor);

		m_width = image.width;
		m_height = image.height;
		return true;
	}
}
