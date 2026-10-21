//
// TextureResource.cpp
//

#include "pch.h"
#include "TextureResource.h"
#include "TgaLoader.h"
#include "GpuUploader.h"

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

	bool TextureResource::Rebuild(DX::DeviceResources& resources)
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
		DX::GpuUploader uploader;
		uploader.Create(device, resources.GetCommandQueue(),
			rowPitch * image.height + D3D12_TEXTURE_DATA_PLACEMENT_ALIGNMENT);
		uploader.CopyTextureAndWait(m_texture.Get(), image.pixelsRgba8.data(),
			image.width, image.height, image.width * 4, DXGI_FORMAT_R8G8B8A8_UNORM);

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
