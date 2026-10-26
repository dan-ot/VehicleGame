//
// GpuContext.h - Narrow, forward-declared handle to the GPU allocator used by asset resources
//
// Only AssetManager, the GPU resource implementations and the Renderer may include DeviceResources.h
// to use the full type. Everything else sees an opaque context.
//

#pragma once

namespace DX
{
	class DeviceResources;
}

namespace Engine
{
	using GpuContext = DX::DeviceResources;
	class AssetUploader;
}
