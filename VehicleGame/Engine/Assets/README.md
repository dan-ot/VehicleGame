# Engine Asset System

The asset system lives in `VehicleGame/Engine/Assets/`. `AssetManager` owns asset lifetime, and each engine mode keeps its own retained `AssetKit`.

## Supported Formats

| Type | Extension | Loader | Residency |
|---|---|---|---|
| Texture | `.tga` (uncompressed 32-bit type 2) | `TgaLoader` | GPU (`TextureResource`) |
| Model | `.gltf` / `.glb` (first mesh primitive) | `GltfLoader` | GPU (`MeshResource`) |
| Sound | `.wav` (RIFF/WAVE 16-bit PCM) | `WavLoader` | CPU (`SoundResource`) |
| Shader | `.hlsl` | `ShaderCompiler` (DXC, HLSL to DXIL) | Compiled at runtime |

Sound stays in CPU memory on purpose. The GPU does not consume it, and reading it back from the GPU would be inefficient.

## Declaring Assets

- Each mode overrides `DeclareAssets(AssetManifest&)` and lists the assets it needs.
- `CoreMode` retains the Loading, Menu, and Play kits. Entering a mode declares its manifest on that mode's kit.
- `CoreMode::RequestTransition(EngineState, AssetManifest)` supplies a manifest for the next state. The manifest is used only when that state is entered, and otherwise the mode's `DeclareAssets` runs. No mode uses this overload yet.

## Device Loss

- `AssetManager` keeps entries across device loss and restores them through `OnDeviceRestored` within the configured budget.
- `TextureResource` and `MeshResource` release GPU objects on loss and rebuild them from their source files.
- `SoundResource` keeps its PCM data, so its release is a no-op.

## Upload and Descriptor Infrastructure

- `UploadBuffer` provides a CPU-writable upload heap for staging copies.
- `GpuUploader` performs synchronous buffer and texture copies on a dedicated allocator, list, and fence. Texture rows are padded to `D3D12_TEXTURE_DATA_PITCH_ALIGNMENT`.
- `DeviceResources` owns a 1024-entry shader-visible CBV/SRV/UAV heap. `AllocateSrvIndex` hands out indices, and `ResetSrvAllocator` clears them.

## Placeholder Assets

Placeholder paths are declared in the mode manifests. Missing files are logged through `OutputDebugStringW`, and the asset is left unloaded without throwing.

- Loading: `Assets\loading_splash.tga`
- Menu: `Assets\menu_background.tga`, `Assets\menu_click.wav`
- Play: `Assets\vehicle.glb`, `Assets\engine.wav`

## Runtime DXC

- `dxcompiler.lib` is linked in every configuration.
- `dxcompiler.dll` and `dxil.dll` are copied from `vcpkg_installed\x64-windows\x64-windows\bin` to the output directory after each build.
- Xbox does not support runtime DXC compilation. Shaders must be precompiled for Xbox builds (see the TODO in `ShaderCompiler.h`).

## Known Follow-Ups

- `LoadingMode` still uses a fixed 2-second timer. Replace it with an asset-ready signal (see the TODO in `LoadingMode.h`).
- Texture and mesh rebuilds allocate new descriptors and buffers each time. A later pass should reuse or release them explicitly.
