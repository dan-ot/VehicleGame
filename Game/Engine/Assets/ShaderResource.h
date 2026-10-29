//
// ShaderResource.h - CPU-resident compiled shader bytecode for a single HLSL source file
//

#pragma once

#include "AssetManager.h"

#include <cstdint>
#include <string>
#include <vector>

namespace Engine
{
	// Bytecode is consumed when pipeline state objects are created, so it stays resident in memory.
	// Device loss does not affect it; Release and Rebuild are no-ops.
	class ShaderResource final : public IAssetResource
	{
	public:
		explicit ShaderResource(std::wstring sourcePath);

		void Release() noexcept override {}
		bool Rebuild(DX::DeviceResources& resources, ID3D12GraphicsCommandList* commandList) override;
		bool Load() override;

		const std::vector<uint8_t>& VertexShader() const noexcept { return m_vertexShader; }
		const std::vector<uint8_t>& PixelShader() const noexcept { return m_pixelShader; }
		bool IsLoaded() const noexcept { return m_loaded; }

	private:
		std::wstring m_sourcePath;
		std::vector<uint8_t> m_vertexShader;
		std::vector<uint8_t> m_pixelShader;
		bool m_loaded = false;
	};
}
