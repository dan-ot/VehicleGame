//
// FontResource.h - GPU-resident glyph atlas asset backed by a TTF source file
//

#pragma once

#include "AssetManager.h"

#include <cstdint>
#include <string>
#include <unordered_map>

namespace Engine
{
	struct GlyphInfo
	{
		float u0 = 0.0f;
		float v0 = 0.0f;
		float u1 = 0.0f;
		float v1 = 0.0f;
		float width = 0.0f;
		float height = 0.0f;
		float bearingX = 0.0f;
		float bearingY = 0.0f;
		float advance = 0.0f;
	};

	struct FontMetrics
	{
		float ascender = 0.0f;
		float descender = 0.0f;
		float lineHeight = 0.0f;
	};

	class FontResource final : public IAssetResource
	{
	public:
		explicit FontResource(std::wstring sourcePath);

		void Release() noexcept override;
		bool Rebuild(DX::DeviceResources& resources, AssetUploader& uploader) override;

		// Returns null when the codepoint was not rasterized into the atlas.
		const GlyphInfo* Find(char32_t codepoint) const noexcept;
		const FontMetrics& Metrics() const noexcept { return m_metrics; }
		D3D12_GPU_DESCRIPTOR_HANDLE GpuDescriptor() const noexcept { return m_gpuDescriptor; }

	private:
		std::wstring m_sourcePath;
		Microsoft::WRL::ComPtr<ID3D12Resource> m_texture;
		D3D12_CPU_DESCRIPTOR_HANDLE m_cpuDescriptor = {};
		D3D12_GPU_DESCRIPTOR_HANDLE m_gpuDescriptor = {};
		std::unordered_map<char32_t, GlyphInfo> m_glyphs;
		FontMetrics m_metrics;
	};
}
