//
// FontResource.cpp
//

#include "pch.h"
#include "FontResource.h"
#include "GlyphAtlasPacker.h"
#include "../../UploadBuffer.h"

#include <ft2build.h>
#include FT_FREETYPE_H

#include <cstdlib>
#include <cstring>
#include <fstream>
#include <iterator>
#include <vector>

namespace
{
	constexpr uint32_t kAtlasSize = 512;
	constexpr uint32_t kAtlasPadding = 1;
	constexpr FT_UInt kPixelSize = 32;
	constexpr char32_t kFirstGlyph = 0x20;
	constexpr char32_t kLastGlyph = 0x7E;

	bool ReadFileBytes(const std::wstring& path, std::vector<uint8_t>& bytes)
	{
		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return false;
		}
		bytes.assign(std::istreambuf_iterator<char>(file), std::istreambuf_iterator<char>());
		return !bytes.empty();
	}

	struct FreeTypeLibrary
	{
		FT_Library handle = nullptr;
		~FreeTypeLibrary() { if (handle) FT_Done_FreeType(handle); }
	};

	struct FreeTypeFace
	{
		FT_Face handle = nullptr;
		~FreeTypeFace() { if (handle) FT_Done_Face(handle); }
	};
}

namespace Engine
{
	FontResource::FontResource(std::wstring sourcePath)
		: m_sourcePath(std::move(sourcePath))
	{
	}

	void FontResource::Release() noexcept
	{
		m_texture.Reset();
		m_cpuDescriptor = {};
		m_gpuDescriptor = {};
		m_glyphs.clear();
		m_metrics = {};
	}

	const GlyphInfo* FontResource::Find(char32_t codepoint) const noexcept
	{
		const auto it = m_glyphs.find(codepoint);
		return it == m_glyphs.end() ? nullptr : &it->second;
	}

	bool FontResource::Rebuild(DX::DeviceResources& resources, AssetUploader& uploader)
	{
		Release();

		std::vector<uint8_t> fontData;
		if (!ReadFileBytes(m_sourcePath, fontData))
		{
			return false;
		}

		FreeTypeLibrary library;
		if (FT_Init_FreeType(&library.handle) != 0)
		{
			return false;
		}

		FreeTypeFace face;
		if (FT_New_Memory_Face(library.handle, fontData.data(), static_cast<FT_Long>(fontData.size()), 0, &face.handle) != 0)
		{
			return false;
		}
		if (FT_Set_Pixel_Sizes(face.handle, 0, kPixelSize) != 0)
		{
			return false;
		}

		std::vector<uint8_t> atlasPixels(static_cast<size_t>(kAtlasSize) * kAtlasSize * 4, 0);
		GlyphAtlasPacker packer(kAtlasSize, kAtlasSize, kAtlasPadding);
		std::unordered_map<char32_t, GlyphInfo> glyphs;

		for (char32_t codepoint = kFirstGlyph; codepoint <= kLastGlyph; ++codepoint)
		{
			if (FT_Load_Char(face.handle, codepoint, FT_LOAD_RENDER) != 0)
			{
				return false;
			}

			const FT_GlyphSlot slot = face.handle->glyph;
			const FT_Bitmap& bitmap = slot->bitmap;

			GlyphInfo info;
			info.width = static_cast<float>(bitmap.width);
			info.height = static_cast<float>(bitmap.rows);
			info.bearingX = static_cast<float>(slot->bitmap_left);
			info.bearingY = static_cast<float>(slot->bitmap_top);
			info.advance = static_cast<float>(slot->advance.x) / 64.0f;

			if (bitmap.width > 0 && bitmap.rows > 0)
			{
				const auto rect = packer.Place(bitmap.width, bitmap.rows);
				if (!rect)
				{
					return false;
				}

				for (unsigned int row = 0; row < bitmap.rows; ++row)
				{
					const unsigned char* sourceRow = bitmap.buffer + static_cast<ptrdiff_t>(row) * bitmap.pitch;
					for (unsigned int col = 0; col < bitmap.width; ++col)
					{
						const size_t dst = (static_cast<size_t>(rect->y + row) * kAtlasSize + rect->x + col) * 4;
						atlasPixels[dst + 0] = 255;
						atlasPixels[dst + 1] = 255;
						atlasPixels[dst + 2] = 255;
						atlasPixels[dst + 3] = sourceRow[col];
					}
				}

				info.u0 = static_cast<float>(rect->x) / kAtlasSize;
				info.v0 = static_cast<float>(rect->y) / kAtlasSize;
				info.u1 = static_cast<float>(rect->x + rect->width) / kAtlasSize;
				info.v1 = static_cast<float>(rect->y + rect->height) / kAtlasSize;
			}

			glyphs.emplace(codepoint, info);
		}

		const FT_Size_Metrics& sizeMetrics = face.handle->size->metrics;
		m_metrics.ascender = static_cast<float>(sizeMetrics.ascender) / 64.0f;
		m_metrics.descender = static_cast<float>(sizeMetrics.descender) / 64.0f;
		m_metrics.lineHeight = static_cast<float>(sizeMetrics.height) / 64.0f;

		auto* device = resources.GetD3DDevice();

		const CD3DX12_HEAP_PROPERTIES heapProps(D3D12_HEAP_TYPE_DEFAULT);
		const CD3DX12_RESOURCE_DESC desc = CD3DX12_RESOURCE_DESC::Tex2D(
			DXGI_FORMAT_R8G8B8A8_UNORM, kAtlasSize, kAtlasSize, 1, 1);

		DX::ThrowIfFailed(device->CreateCommittedResource(
			&heapProps,
			D3D12_HEAP_FLAG_NONE,
			&desc,
			D3D12_RESOURCE_STATE_COPY_DEST,
			nullptr,
			IID_PPV_ARGS(m_texture.ReleaseAndGetAddressOf())));

		const size_t rowPitch = (static_cast<size_t>(kAtlasSize) * 4 + D3D12_TEXTURE_DATA_PITCH_ALIGNMENT - 1) /
			D3D12_TEXTURE_DATA_PITCH_ALIGNMENT * D3D12_TEXTURE_DATA_PITCH_ALIGNMENT;
		const size_t totalBytes = rowPitch * kAtlasSize;

		std::vector<uint8_t> padded(totalBytes, 0);
		for (UINT row = 0; row < kAtlasSize; ++row)
		{
			std::memcpy(padded.data() + row * rowPitch, atlasPixels.data() + static_cast<size_t>(row) * kAtlasSize * 4,
				static_cast<size_t>(kAtlasSize) * 4);
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
		uploader.ExecuteAndWait([dest, source, offset, rowPitch](ID3D12GraphicsCommandList* commandList) {
			D3D12_TEXTURE_COPY_LOCATION src = {};
			src.pResource = source;
			src.Type = D3D12_TEXTURE_COPY_TYPE_PLACED_FOOTPRINT;
			src.PlacedFootprint.Offset = offset;
			src.PlacedFootprint.Footprint.Format = DXGI_FORMAT_R8G8B8A8_UNORM;
			src.PlacedFootprint.Footprint.Width = kAtlasSize;
			src.PlacedFootprint.Footprint.Height = kAtlasSize;
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

		m_glyphs = std::move(glyphs);
		return true;
	}
}
