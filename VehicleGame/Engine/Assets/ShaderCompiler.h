//
// ShaderCompiler.h - Runtime HLSL compilation through DXC
//

#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace Engine
{
	// Compiles an HLSL source file to DXIL. Returns false and leaves bytecode empty on failure.
	// TODO: Xbox does not support runtime DXC compilation; shaders must be precompiled for Xbox builds.
	bool CompileShader(const std::wstring& path, const char* entryPoint, const char* target, std::vector<uint8_t>& bytecode);
}
