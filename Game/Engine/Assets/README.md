# Engine Asset System

The asset system lives in `Game/Engine/Assets/`.

## Supported Formats

| Type | Extension | Loader | Residency |
|---|---|---|---|
| Texture | `.tga` (uncompressed 32-bit type 2) | `TgaLoader` | GPU (`TextureResource`) |
| Model | `.gltf` / `.glb` (first mesh primitive) | `GltfLoader` | GPU (`MeshResource`) |
| Sound | `.wav` (RIFF/WAVE 16-bit PCM) | `WavLoader` | CPU (`SoundResource`) |
| Music | `.wav` (RIFF/WAVE 16-bit PCM, streamed) | `WavStreamReader` | CPU metadata (`MusicResource`), audio streamed on demand |
| Shader | `.hlsl` (`VSMain` and `PSMain`) | `ShaderCompiler` (DXC, HLSL to DXIL) | CPU (`ShaderResource`), compiled at runtime |

Sound and Music stay in CPU memory on purpose. The GPU does not consume them, and reading them back from the GPU would be inefficient.

## Typed Accessors

- `AssetManager` keeps one residency map keyed by `AssetKey` and exposes typed lookups: `FindTexture`, `FindModel`, `FindFont`, `FindSound`, and `FindMusic`.
- Consumers use the typed accessors. The generic `Find` is private to `AssetManager`.
- Renderers and the Mixer receive `AssetKey`s and never handle file paths or loading.

## Build Timing

- CPU assets (`Sound`, `Music`, `Shader`) are built immediately in `AssetManager::Request` through `AssetLoader::CreateCpu`, which needs no `GpuContext`.
- GPU assets (`Texture`, `Model`, `Font`) are built through `AssetLoader::CreateAndBuild` with a `GpuContext`.

## Declaring Assets

- Each mode overrides `DeclareAssets(AssetManifest&)` and lists the assets it needs.
- `CoreMode` retains the Loading, Menu, and Play kits. Entering a mode declares its manifest on that mode's kit.
- `ModeDescriptors::ManifestFor(ModeId)` returns a destination's manifest without constructing the mode, so transitions can stage assets before the mode exists.
- `CoreMode::RequestTransition(ModeId)` stages the destination's manifest and constructs the mode only after its assets are built or have failed.

## Persistent Requesters

- `RequesterId` identifies either an `EngineState` or a `PersistentOwner`. Each `AssetManager` entry tracks its requesters in a set.
- `Mixer` holds a persistent `AssetKit` under `PersistentOwner::Mixer`. Its assets survive mode transitions until the Mixer releases them.

## Audio

- The `Game/Engine/Audio/` folder contains the XAudio2 `Mixer` (`Mixer.h`, `Mixer.cpp`) and the XAudio2-free `MixerTypes.h`.
- `Mixer` uses XAudio2 through a pImpl, so XAudio2 headers stay out of public headers.
- Channels are Music, System, Gameplay, Background, and Other. Gameplay and Other stop on mode exit. Music, System, and Background persist.

## Device Loss

- `AssetManager` keeps entries across device loss and restores them through `OnDeviceRestored` within the configured budget.
- `ShaderResource` keeps its bytecode in memory, so its release is a no-op.
- `TextureResource` and `MeshResource` release GPU objects on loss and rebuild them from their source files.
- `SoundResource` keeps its PCM data, so its release is a no-op.

## Upload and Descriptor Infrastructure

- `UploadBuffer` provides a CPU-writable upload heap for staging copies.
- `GpuUploader` performs synchronous buffer and texture copies on a dedicated allocator, list, and fence. Texture rows are padded to `D3D12_TEXTURE_DATA_PITCH_ALIGNMENT`.
- `DeviceResources` owns a 1024-entry shader-visible CBV/SRV/UAV heap. `AllocateSrvIndex` hands out indices, and `ResetSrvAllocator` clears them.

## Placeholder Assets

Placeholder paths are declared in the mode manifests. Missing files are logged through `OutputDebugStringW`, and the asset is left unloaded without throwing.

- Loading: `Assets\loading_splash.tga`, `Assets\loading_logo.tga`, `Assets\loading_bar_fill.tga`, `Assets\loading_spinner.tga`, `Assets\cursor.tga`
- Menu: `Assets\menu_background.tga`, `Assets\menu_click.wav`
- Play: `Assets\vehicle.glb`, `Assets\engine.wav`

## Runtime DXC

- `dxcompiler.lib` is linked in every configuration.
- `dxcompiler.dll` and `dxil.dll` are copied from `vcpkg_installed\x64-windows\x64-windows\bin` to the output directory after each build.
- `Engine/Render/QuadShader.hlsl` is loaded through `AssetManager` as a `ShaderResource` by `QuadRenderer`, which holds it under `PersistentOwner::Renderer`. It is compiled at runtime and is declared as a `None` project item, so it has no precompiled output. The compile blocks briefly during renderer construction.
- Xbox does not support runtime DXC compilation. Shaders must be precompiled for Xbox builds (see the TODO in `ShaderCompiler.h`).

## Known Follow-Ups

- `LoadingMode` still uses a fixed 2-second timer. Replace it with an asset-ready signal (see the TODO in `LoadingMode.h`).
- Texture and mesh rebuilds allocate new descriptors and buffers each time. A later pass should reuse or release them explicitly.
