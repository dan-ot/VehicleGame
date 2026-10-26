//
// QuadRenderer.cpp - Renderer implementation (Direct3D 12)
//

#include "pch.h"
#include "QuadRenderer.h"
#include "../../DeviceResources.h"
#include "../Assets/AssetManager.h"
#include "../Assets/FontResource.h"
#include "../Assets/ShaderCompiler.h"
#include "../Assets/TextureResource.h"

#include <pix3.h>
#include <stdexcept>

namespace Engine
{
	namespace
	{
		constexpr wchar_t kShaderPath[] = L"Engine\\Render\\QuadShader.hlsl";
		constexpr UINT kConstantCount = 16;
		constexpr double kAssetRestoreBudgetSeconds = 0.002;
	}

	struct Renderer::Impl
	{
		Impl(DX::DeviceResources& deviceResources, AssetManager& assetManager)
			: resources(deviceResources), assets(assetManager)
		{
		}

		D3D12_GPU_DESCRIPTOR_HANDLE Resolve(const AssetKey& key) const
		{
			const IAssetResource* resource = assets.Find(key);
			if (const auto* texture = dynamic_cast<const TextureResource*>(resource))
			{
				return texture->GpuDescriptor();
			}
			if (const auto* font = dynamic_cast<const FontResource*>(resource))
			{
				return font->GpuDescriptor();
			}
			return {};
		}

		DX::DeviceResources& resources;
		AssetManager& assets;
		Microsoft::WRL::ComPtr<ID3D12RootSignature> rootSignature;
		Microsoft::WRL::ComPtr<ID3D12PipelineState> pipelineState;
	};

	Renderer::Renderer(DX::DeviceResources& resources, AssetManager& assets)
		: m_impl(std::make_unique<Impl>(resources, assets))
	{
		CreateDeviceResources();
	}

	Renderer::~Renderer() = default;

	void Renderer::OnDeviceLost() noexcept
	{
		m_impl->rootSignature.Reset();
		m_impl->pipelineState.Reset();
	}

	void Renderer::CreateDeviceResources()
	{
		std::vector<uint8_t> vertexShader;
		std::vector<uint8_t> pixelShader;
		if (!CompileShader(kShaderPath, "VSMain", "vs_6_0", vertexShader) ||
			!CompileShader(kShaderPath, "PSMain", "ps_6_0", pixelShader))
		{
			throw std::runtime_error("Renderer: failed to compile QuadShader.hlsl");
		}

		CD3DX12_DESCRIPTOR_RANGE srvRange;
		srvRange.Init(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, 1, 0);

		CD3DX12_ROOT_PARAMETER parameters[2];
		parameters[0].InitAsConstants(kConstantCount, 0, 0, D3D12_SHADER_VISIBILITY_ALL);
		parameters[1].InitAsDescriptorTable(1, &srvRange, D3D12_SHADER_VISIBILITY_PIXEL);

		const CD3DX12_STATIC_SAMPLER_DESC sampler(0, D3D12_FILTER_MIN_MAG_MIP_LINEAR,
			D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP, D3D12_TEXTURE_ADDRESS_MODE_WRAP);

		const CD3DX12_ROOT_SIGNATURE_DESC rootDesc(_countof(parameters), parameters, 1, &sampler,
			D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

		Microsoft::WRL::ComPtr<ID3DBlob> serialized;
		Microsoft::WRL::ComPtr<ID3DBlob> errors;
		DX::ThrowIfFailed(D3D12SerializeRootSignature(&rootDesc, D3D_ROOT_SIGNATURE_VERSION_1,
			serialized.GetAddressOf(), errors.GetAddressOf()));

		auto& resources = m_impl->resources;
		auto* device = resources.GetD3DDevice();
		DX::ThrowIfFailed(device->CreateRootSignature(0, serialized->GetBufferPointer(), serialized->GetBufferSize(),
			IID_PPV_ARGS(m_impl->rootSignature.ReleaseAndGetAddressOf())));

		D3D12_GRAPHICS_PIPELINE_STATE_DESC desc = {};
		desc.pRootSignature = m_impl->rootSignature.Get();
		desc.VS = { vertexShader.data(), vertexShader.size() };
		desc.PS = { pixelShader.data(), pixelShader.size() };
		desc.BlendState = CD3DX12_BLEND_DESC(D3D12_DEFAULT);
		desc.BlendState.RenderTarget[0].BlendEnable = TRUE;
		desc.BlendState.RenderTarget[0].SrcBlend = D3D12_BLEND_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].DestBlend = D3D12_BLEND_INV_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].BlendOp = D3D12_BLEND_OP_ADD;
		desc.BlendState.RenderTarget[0].SrcBlendAlpha = D3D12_BLEND_ONE;
		desc.BlendState.RenderTarget[0].DestBlendAlpha = D3D12_BLEND_INV_SRC_ALPHA;
		desc.BlendState.RenderTarget[0].BlendOpAlpha = D3D12_BLEND_OP_ADD;
		desc.SampleMask = UINT_MAX;
		desc.RasterizerState = CD3DX12_RASTERIZER_DESC(D3D12_DEFAULT);
		desc.RasterizerState.CullMode = D3D12_CULL_MODE_NONE;
		desc.DepthStencilState = CD3DX12_DEPTH_STENCIL_DESC(D3D12_DEFAULT);
		desc.DepthStencilState.DepthEnable = FALSE;
		desc.InputLayout = { nullptr, 0 };
		desc.PrimitiveTopologyType = D3D12_PRIMITIVE_TOPOLOGY_TYPE_TRIANGLE;
		desc.NumRenderTargets = 1;
		desc.RTVFormats[0] = resources.GetBackBufferFormat();
		desc.DSVFormat = resources.GetDepthBufferFormat();
		desc.SampleDesc.Count = 1;

		DX::ThrowIfFailed(device->CreateGraphicsPipelineState(&desc,
			IID_PPV_ARGS(m_impl->pipelineState.ReleaseAndGetAddressOf())));
	}

	void Renderer::BeginFrame()
	{
		auto& resources = m_impl->resources;

		resources.Prepare();
		m_impl->assets.RestoreWithinBudget(resources, kAssetRestoreBudgetSeconds);

		auto* commandList = resources.GetCommandList();
		PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Clear");

		const auto rtvDescriptor = resources.GetRenderTargetView();
		const auto dsvDescriptor = resources.GetDepthStencilView();

		commandList->OMSetRenderTargets(1, &rtvDescriptor, FALSE, &dsvDescriptor);
		commandList->ClearRenderTargetView(rtvDescriptor, DirectX::Colors::CornflowerBlue, 0, nullptr);
		commandList->ClearDepthStencilView(dsvDescriptor, D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);

		const auto viewport = resources.GetScreenViewport();
		const auto scissor = resources.GetScissorRect();
		commandList->RSSetViewports(1, &viewport);
		commandList->RSSetScissorRects(1, &scissor);

		PIXEndEvent(commandList);

		PIXBeginEvent(commandList, PIX_COLOR_DEFAULT, L"Render");

		ID3D12DescriptorHeap* heaps[] = { resources.GetSrvDescriptorHeap() };
		commandList->SetDescriptorHeaps(1, heaps);
		commandList->SetGraphicsRootSignature(m_impl->rootSignature.Get());
		commandList->SetPipelineState(m_impl->pipelineState.Get());
		commandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLESTRIP);
	}

	void Renderer::EndFrame()
	{
		auto& resources = m_impl->resources;

		PIXEndEvent(resources.GetCommandList());

		PIXBeginEvent(resources.GetCommandQueue(), PIX_COLOR_DEFAULT, L"Present");
		resources.Present();
		PIXEndEvent(resources.GetCommandQueue());
	}

	float Renderer::ViewportWidth() const noexcept
	{
		return m_impl->resources.GetScreenViewport().Width;
	}

	float Renderer::ViewportHeight() const noexcept
	{
		return m_impl->resources.GetScreenViewport().Height;
	}

	bool Renderer::IsDrawable(const AssetKey& key) const
	{
		return m_impl->Resolve(key).ptr != 0;
	}

	void Renderer::Draw(const AssetKey& key, const RectF& destination, const UvRectF& uv, const ColorF& tint)
	{
		const D3D12_GPU_DESCRIPTOR_HANDLE texture = m_impl->Resolve(key);
		if (texture.ptr == 0)
		{
			return;
		}

		auto* commandList = m_impl->resources.GetCommandList();
		const float constants[kConstantCount] =
		{
			destination.x, destination.y, destination.width, destination.height,
			uv.u0, uv.v0, uv.u1, uv.v1,
			tint.r, tint.g, tint.b, tint.a,
			ViewportWidth(), ViewportHeight(), 0.0f, 0.0f
		};
		commandList->SetGraphicsRoot32BitConstants(0, kConstantCount, constants, 0);
		commandList->SetGraphicsRootDescriptorTable(1, texture);
		commandList->DrawInstanced(4, 1, 0, 0);
	}
}
