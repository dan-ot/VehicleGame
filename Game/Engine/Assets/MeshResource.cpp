//
// MeshResource.cpp
//

#include "pch.h"
#include "MeshResource.h"
#include "AssetUploader.h"
#include "../../DeviceResources.h"
#include "GltfLoader.h"
#include "GpuUploader.h"

namespace Engine
{
	MeshResource::MeshResource(std::wstring sourcePath)
		: m_sourcePath(std::move(sourcePath))
	{
	}

	void MeshResource::Release() noexcept
	{
		m_vertexBuffer.Reset();
		m_indexBuffer.Reset();
		m_vbView = {};
		m_ibView = {};
		m_indexCount = 0;
	}

	bool MeshResource::Rebuild(DX::DeviceResources& resources, AssetUploader&)
	{
		Release();

		DecodedModel model;
		if (!LoadGltf(m_sourcePath, model) || model.vertices.empty() || model.indices.empty())
		{
			return false;
		}

		auto* device = resources.GetD3DDevice();
		const size_t vertexBytes = model.vertices.size() * sizeof(DecodedVertex);
		const size_t indexBytes = model.indices.size() * sizeof(uint32_t);

		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);

		const CD3DX12_RESOURCE_DESC vbDesc = CD3DX12_RESOURCE_DESC::Buffer(vertexBytes);
		DX::ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&vbDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_vertexBuffer.ReleaseAndGetAddressOf())));

		const CD3DX12_RESOURCE_DESC ibDesc = CD3DX12_RESOURCE_DESC::Buffer(indexBytes);
		DX::ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&ibDesc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_indexBuffer.ReleaseAndGetAddressOf())));

		DX::GpuUploader uploader;
		uploader.Create(device, resources.GetCommandQueue(), vertexBytes > indexBytes ? vertexBytes : indexBytes);
		uploader.CopyBufferAndWait(m_vertexBuffer.Get(), model.vertices.data(), vertexBytes);
		uploader.CopyBufferAndWait(m_indexBuffer.Get(), model.indices.data(), indexBytes);

		m_vbView.BufferLocation = m_vertexBuffer->GetGPUVirtualAddress();
		m_vbView.SizeInBytes = static_cast<UINT>(vertexBytes);
		m_vbView.StrideInBytes = sizeof(DecodedVertex);

		m_ibView.BufferLocation = m_indexBuffer->GetGPUVirtualAddress();
		m_ibView.SizeInBytes = static_cast<UINT>(indexBytes);
		m_ibView.Format = DXGI_FORMAT_R32_UINT;

		m_indexCount = static_cast<UINT>(model.indices.size());
		return true;
	}
}
