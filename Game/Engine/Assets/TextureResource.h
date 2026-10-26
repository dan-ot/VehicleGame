//
// TextureResource.h - GPU-resident texture asset backed by a TGA source file
//

#pragma once

#include "AssetManager.h"
#include "AssetUploader.h"

#include <string>

namespace Engine
{
	class TextureResource final : public IAssetResource
	{
	public:
		explicit TextureResource(std::wstring sourcePath);

		void Release() noexcept override;
		bool Rebuild(DX::DeviceResources& resources, AssetUploader& uploader) override;

		D3D12_CPU_DESCRIPTOR_HANDLE CpuDescriptor() const noexcept { return m_cpuDescriptor; }
		D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor() const noexcept { return m_gpuDescriptor; }
		uint32_t Width() const noexcept { return m_width; }
		uint32_t Height() const noexcept { return m_height; }

	private:
		std::wstring m_sourcePath;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptor = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptor = {};
		uint32_t m_width = 0;
		uint32_t m_height = 0;
	};
}
