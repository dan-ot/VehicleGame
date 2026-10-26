//
// QuadRenderer.h - Renderer: the only consumer of GPU primitives for 2D drawing
//
// Callers supply AssetKeys; the Renderer resolves them through the AssetManager. This header is
// intentionally free of DirectX types.
//

#pragma once

#include "../Assets/AssetKey.h"
#include "Primitives.h"
#include "RenderTypes.h"

#include <memory>

namespace DX
{
	class DeviceResources;
}

namespace Engine
{
	class AssetManager;

	class Renderer
	{
	public:
		Renderer(DX::DeviceResources& resources, AssetManager& assets);
		~Renderer();

		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

		// Compiles shaders and recreates the root signature and pipeline state for the current device.
		void CreateDeviceResources();
		void OnDeviceLost() noexcept;

		// Prepares the back buffer, restores queued assets within budget, clears and binds the quad pipeline.
		void BeginFrame();
		// Finishes the frame and presents.
		void EndFrame();

		float ViewportWidth() const noexcept;
		float ViewportHeight() const noexcept;

		// True when the key refers to a resident asset the Renderer can draw (texture or font atlas).
		bool IsDrawable(const AssetKey& key) const;

		// Draws one quad sampled from the keyed texture or font atlas. Skipped when the key is not drawable.
		// Uvs outside [0,1] wrap.
		void Draw(const AssetKey& key, const RectF& destination, const UvRectF& uv, const ColorF& tint);

	private:
		struct Impl;
		std::unique_ptr<Impl> m_impl;
	};
}
