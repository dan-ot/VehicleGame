//
// ShaderCompiler.cpp
//

#include "pch.h"
#include "ShaderCompiler.h"

#include <wrl/client.h>
#include <directx-dxc/dxcapi.h>

#include <cstring>
#include <fstream>
#include <iterator>

namespace Engine
{
	bool CompileShader(const std::wstring& path, const char* entryPoint, const char* target, std::vector<uint8_t>& bytecode)
	{
		bytecode.clear();

		std::ifstream file(path, std::ios::binary);
		if (!file)
		{
			return false;
		}
		std::string source((std::istreambuf_iterator<char>(file)), std::istreambuf_iterator<char>());

		Microsoft::WRL::ComPtr<IDxcUtils> utils;
		Microsoft::WRL::ComPtr<IDxcCompiler3> compiler;
		if (FAILED(DxcCreateInstance(CLSID_DxcUtils, IID_PPV_ARGS(utils.GetAddressOf()))) ||
			FAILED(DxcCreateInstance(CLSID_DxcCompiler, IID_PPV_ARGS(compiler.GetAddressOf()))))
		{
			return false;
		}

		DxcBuffer sourceBuffer = {};
		sourceBuffer.Ptr = source.data();
		sourceBuffer.Size = source.size();
		sourceBuffer.Encoding = DXC_CP_UTF8;

		std::wstring wideEntry(entryPoint, entryPoint + strlen(entryPoint));
		std::wstring wideTarget(target, target + strlen(target));
		LPCWSTR args[] = { L"-E", wideEntry.c_str(), L"-T", wideTarget.c_str() };

		Microsoft::WRL::ComPtr<IDxcResult> result;
		if (FAILED(compiler->Compile(&sourceBuffer, args, static_cast<UINT32>(std::size(args)), nullptr, IID_PPV_ARGS(result.GetAddressOf()))))
		{
			return false;
		}

		HRESULT status = E_FAIL;
		result->GetStatus(&status);
		if (FAILED(status))
		{
			return false;
		}

		Microsoft::WRL::ComPtr<IDxcBlob> object;
		if (FAILED(result->GetOutput(DXC_OUT_OBJECT, IID_PPV_ARGS(object.GetAddressOf()), nullptr)) || !object)
		{
			return false;
		}

		const uint8_t* begin = static_cast<const uint8_t*>(object->GetBufferPointer());
		bytecode.assign(begin, begin + object->GetBufferSize());
		return true;
	}
}
