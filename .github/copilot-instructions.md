# VehicleGame – Project Structure Guide

This file describes the layout of the VehicleGame workspace so planning and coding agents can make accurate assumptions about where code lives.

## Overview

- **Type:** Native C++ Windows game, Direct3D 12, built with MSBuild (Visual Studio C++ project).
- **Solution:** `VehicleGame.slnx` (XML solution format) at the repository root. All project paths in it are relative to the folder containing this file, e.g. `Game/Game.vcxproj` (project file at `Game\Game.vcxproj`). Resolve project paths against the repository root, not the `Game\` project folder.
- **Platform:** Windows x64 (Debug and Release configurations). Targets the Microsoft GDK (Xbox PC / Microsoft Store).
- **Package management:** vcpkg in manifest mode.
- **Language standard:** Modern C++ (C++17 or later). Precompiled headers are used.

## Repository Layout

```
VehicleGame/                      <- Repository / solution root
├── VehicleGame.slnx              <- Solution file
├── Game/                         <- Main C++ project (all source lives here)
│   ├── Game.vcxproj              <- MSBuild project file (add new source files here)
│   ├── Game.vcxproj.filters
│   ├── vcpkg.json                <- vcpkg manifest (dependencies)
│   ├── vcpkg-configuration.json  <- vcpkg registry configuration
│   ├── README.md
│   ├── Main.cpp                  <- Entry point, Win32 window procedure
│   ├── Game.h / Game.cpp         <- Top-level Game class (update/render loop)
│   ├── DeviceResources.h / .cpp  <- D3D12 device, swap chain, command queue
│   ├── GpuUploader.h / .cpp      <- Synchronous GPU buffer/texture copies
│   ├── UploadBuffer.h            <- CPU-writable upload heap
│   ├── StepTimer.h               <- Fixed/variable timestep helper
│   ├── pch.h / pch.cpp           <- Precompiled header
│   ├── resource.rc               <- Win32 resources
│   ├── settings.manifest         <- Application manifest
│   ├── MicrosoftGameConfig.mgc   <- GDK game configuration
│   ├── PackageLayout.ps1 / .flt  <- Loose layout packaging script and exclusion list
│   ├── Assets/                   <- Store/splash logo images
│   ├── Engine/                   <- Game engine/state framework
│   │   ├── Assets/               <- Asset manager, loaders, GPU/sound resources, shader compiler
│   │   ├── Layout/               <- Layout tree and layouts (e.g. SplashLayout)
│   │   ├── Render/               <- Renderers, render helpers, SoftwareCursor.h, QuadShader.hlsl
│   │   ├── Scene/                <- Scene
│   │   ├── State/                <- Engine state, modes base, input system
│   │   │   ├── EngineState.h     <- Global engine state container
│   │   │   ├── EngineMode.h      <- Abstract mode interface
│   │   │   ├── CoreMode.h        <- Shared base for modes
│   │   │   ├── ModeHandoff.h     <- Mode transition handoff
│   │   │   ├── ModeTimer.h       <- Timers tied to a mode
│   │   │   ├── LoadingTracker.h  <- Loading progress tracking
│   │   │   ├── InputCommands.h   <- Input command definitions
│   │   │   ├── InputDevices.h    <- Device-specific input
│   │   │   ├── InputFrame.h      <- Per-frame input snapshot
│   │   │   ├── InputMap.h        <- Input-to-command mapping
│   │   │   └── InputSystem.h     <- Input polling/dispatch
│   │   └── Modes/                <- Concrete game modes/screens
│   │       ├── LoadingMode.h     <- Owns a SoftwareCursor
│   │       ├── MenuMode.h        <- Owns a SoftwareCursor
│   │       └── PlayMode.h
│   └── (x64/, ARM64/)            <- Build output (ignore; never edit)
└── x64/Debug/                    <- Build output and copied runtime DLLs/assets (ignore)
```

## Where Things Go

| Kind of change | Location |
|---|---|
| Application startup / window handling | `Game/Main.cpp` |
| Game loop, update and render logic | `Game/Game.h`, `Game/Game.cpp` |
| Direct3D 12 setup, swap chain, resources | `Game/DeviceResources.h/.cpp` |
| New game mode or screen | `Game/Engine/Modes/<Name>Mode.h` (derive from `CoreMode`/`EngineMode`) |
| Mode-independent engine state | `Game/Engine/State/` |
| Input handling | `Game/Engine/State/Input*.h` |
| Timing utilities | `Game/StepTimer.h` or `Game/Engine/State/ModeTimer.h` |
| Precompiled header includes | `Game/pch.h` |
| Dependencies (libraries, SDKs) | `Game/vcpkg.json` |
| Win32 resources, icons, manifests | `Game/resource.rc`, `Game/MicrosoftGameConfig.mgc` |
| Asset loading, GPU resources | `Game/Engine/Assets/` |
| Shaders (if added) | `Game/Engine/Render/` alongside other render code, registered in `Game.vcxproj` as HLSL |
| Image assets | Under `Game/Assets/`, copied on build |

## Rendering Boundary

- Only `Engine/Render/QuadRenderer.*` (class `Renderer`), `Engine/Assets/*` GPU resources/uploader, `DeviceResources`, `GpuUploader`/`UploadBuffer`, `Game.*` and `Main.cpp` may include DirectX headers. `pch.h` contains no DirectX headers; include them directly where needed.
- Modes, menus and primitives hold `AssetKey`s and `RectF`/`UvRectF`/`ColorF` (`Engine/Render/RenderTypes.h`) and call `Renderer::Draw(key, ...)`.
- Font metrics for layout come from `IFontData` (`Engine/Assets/FontData.h`) via `AssetManager::FindFont`.
- `Renderer` owns the frame lifecycle (`BeginFrame`/`EndFrame`); `AssetManager` owns GPU resource lifetime.

## Conventions

- Headers and sources are flat in `Game/` for core framework files; engine code is grouped under `Game/Engine/`.
- Header-only helpers (e.g. `StepTimer.h`, engine `.h` files) are common; a `.cpp` is only added when implementation is non-trivial.
- Every new `.cpp` must be added to `Game.vcxproj` (and to `Game.vcxproj.filters` to match the folder structure).
- Every new `.cpp` must include `pch.h` first.
- Namespaces and class names use PascalCase; member variables are typically prefixed with `m_`.
- Mode classes derive from the engine mode base and are registered with the engine state rather than instantiated ad hoc.

## Build and Dependencies

- Build with Visual Studio or `msbuild Game\Game.vcxproj /p:Configuration=Debug /p:Platform=x64`.
- Add dependencies by editing `Game/vcpkg.json`; vcpkg restores them during build.
- PlayFab support is optional and enabled through the `ms-gdk` feature `playfab` (see README); it requires uncommenting includes in `pch.h`.
- Do not modify files under `x64/` or any `*.obj`, `*.pdb`, `*.log`, or `*.dll` output.

## Planning Guidance

- When planning a feature, assume the main logic lives in `Game/` and engine-level changes go in `Game/Engine/`.
- Plan changes to new files as: create the file, add it to `Game.vcxproj`, add it to `Game.vcxproj.filters`, then update includes.
- Gameplay or screen work usually touches: a new mode in `Engine/Modes/`, registration in engine state, and `Game.cpp` for loop integration.
- Rendering work usually touches `DeviceResources` and `Game.cpp`.
- Input work usually touches `Engine/State/Input*.h`.
