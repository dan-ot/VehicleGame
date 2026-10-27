//
// FontResource.h - GPU-resident glyph atlas asset backed by a TTF source file
//

#pragma once

#include "AssetManager.h"
#include "AssetUploader.h"
#include "FontData.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Engine
{
	class FontResource final : public IAssetResource, public IFontData
	{
	public:
		explicit FontResource(std::wstring sourcePath);

		void Release() noexcept override;
		bool Rebuild(GpuContext& resources, ID3D12GraphicsCommandList* commandList) override;

		const GlyphInfo* Find(char32_t codepoint) const noexcept override;
		const FontMetrics& Metrics() const noexcept override { return m_metrics; }
		D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor() const noexcept { return m_gpuDescriptor; }

	private:
		std::wstring m_sourcePath;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_staging;
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptor = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptor = {};
		std::unordered_map<char32_t, GlyphInfo> m_glyphs;
		FontMetrics m_metrics;
	};
}
