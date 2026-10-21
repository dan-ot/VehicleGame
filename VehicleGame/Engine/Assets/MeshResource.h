//
// MeshResource.h - GPU-resident mesh asset backed by a glTF/GLB source file
//

#pragma once

#include "AssetManager.h"

#include <string>

namespace Engine
{
	class MeshResource final : public IAssetResource
	{
	public:
		explicit MeshResource(std::wstring sourcePath);

		void Release() noexcept override;
		bool Rebuild(DX::DeviceResources& resources) override;

		D3D12_VERTEX_BUFFER_VIEW VertexBufferView() const noexcept { return m_vbView; }
		D3D12_INDEX_BUFFER_VIEW IndexBufferView() const noexcept { return m_ibView; }
		UINT IndexCount() const noexcept { return m_indexCount; }

	private:
		std::wstring m_sourcePath;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_vertexBuffer;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_indexBuffer;
		D3D12_VERTEX_BUFFER_VIEW m_vbView = {};
		D3D12_INDEX_BUFFER_VIEW m_ibView = {};
		UINT m_indexCount = 0;
	};
}
