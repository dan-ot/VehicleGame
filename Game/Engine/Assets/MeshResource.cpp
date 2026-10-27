//
// MeshResource.cpp
//

#include "pch.h"
#include "MeshResource.h"
#include "AssetUploader.h"
#include "../../DeviceResources.h"
#include "GltfLoader.h"

#include <cstring>

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
		m_vertexStaging.Reset();
		m_indexStaging.Reset();
		m_vbView = {};
		m_ibView = {};
		m_indexCount = 0;
	}

	bool MeshResource::Rebuild(DX::DeviceResources& resources, ID3D12GraphicsCommandList* commandList)
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
		const CD3DX12_HEAP_PROPERTIES uploadProps(D3D12_HEAP_TYPE_UPLOAD);

		const auto createBuffer = [device](const CD3DX12_HEAP_PROPERTIES& props, size_t bytes, D3D12_RESOURCE_STATES state,
			ID3D12Resource** out) {
			const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Buffer(bytes);
			DX::ThrowIfFailed(device->CreateCommittedResource(&props, D3D12_HEAP_FLAG_NONE, &desc, state, nullptr,
				IID_PPV_ARGS(out)));
		};

		createBuffer(heapProps, vertexBytes, D3D12_RESOURCE_STATE_COPY_DEST, m_vertexBuffer.ReleaseAndGetAddressOf());
		createBuffer(heapProps, indexBytes, D3D12_RESOURCE_STATE_COPY_DEST, m_indexBuffer.ReleaseAndGetAddressOf());
		createBuffer(uploadProps, vertexBytes, D3D12_RESOURCE_STATE_GENERIC_READ, m_vertexStaging.ReleaseAndGetAddressOf());
		createBuffer(uploadProps, indexBytes, D3D12_RESOURCE_STATE_GENERIC_READ, m_indexStaging.ReleaseAndGetAddressOf());

		const auto fill = [](ID3D12Resource* staging, const void* data, size_t bytes) {
			void* mapped = nullptr;
			DX::ThrowIfFailed(staging->Map(0, nullptr, &mapped));
			std::memcpy(mapped, data, bytes);
			staging->Unmap(0, nullptr);
		};
		fill(m_vertexStaging.Get(), model.vertices.data(), vertexBytes);
		fill(m_indexStaging.Get(), model.indices.data(), indexBytes);

		commandList->CopyBufferRegion(m_vertexBuffer.Get(), 0, m_vertexStaging.Get(), 0, vertexBytes);
		commandList->CopyBufferRegion(m_indexBuffer.Get(), 0, m_indexStaging.Get(), 0, indexBytes);

		const D3D12_RESOURCE_BARRIER barriers[] = {
			CD3DX12_RESOURCE_BARRIER::Transition(m_vertexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER),
			CD3DX12_RESOURCE_BARRIER::Transition(m_indexBuffer.Get(), D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_INDEX_BUFFER),
		};
		commandList->ResourceBarrier(_countof(barriers), barriers);

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

