//
// QuadRenderer.h - Shared textured-quad pipeline for 2D primitives
//

#pragma once

#include "../../DeviceResources.h"
#include "Primitives.h"

#include <wrl/client.h>

namespace Engine
{
	struct UvRectF
	{
		float u0 = 0.0f;
		float v0 = 0.0f;
		float u1 = 1.0f;
		float v1 = 1.0f;
	};

	struct ColorF
	{
		float r = 1.0f;
		float g = 1.0f;
		float b = 1.0f;
		float a = 1.0f;
	};

	class QuadRenderer
	{
	public:
		explicit QuadRenderer(DX::DeviceResources& resources);

		// Compiles shaders and recreates the root signature and pipeline state for the current device.
		void CreateDeviceResources();

		// Draws one textured quad into the current render target. Uvs outside [0,1] wrap.
		void Draw(D3D12_GPU_DESCRIPTOR_HANDLE texture, const RectF& destination, const UvRectF& uv, const ColorF& tint);

	private:
		DX::DeviceResources& m_resources;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> m_rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> m_pipelineState;
	};
}
