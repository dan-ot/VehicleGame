//
// ShaderResource.cpp
//

#include "pch.h"
#include "ShaderResource.h"
#include "ShaderCompiler.h"

namespace Engine
{
	ShaderResource::ShaderResource(std::wstring sourcePath) : m_sourcePath(std::move(sourcePath)) {}

	bool ShaderResource::Rebuild(DX::DeviceResources&, ID3D12GraphicsCommandList*)
	{
		return true;
	}

	bool ShaderResource::Load()
	{
		if (m_loaded)
		{
			return true;
		}

		m_loaded = CompileShader(m_sourcePath, "VSMain", "vs_6_0", m_vertexShader) &&
			CompileShader(m_sourcePath, "PSMain", "ps_6_0", m_pixelShader);
		if (!m_loaded)
		{
			m_vertexShader.clear();
			m_pixelShader.clear();
		}
		return m_loaded;
	}
}
