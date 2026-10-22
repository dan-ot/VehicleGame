# VehicleGame – Project Structure Guide

This file describes the layout of the VehicleGame workspace so planning and coding agents can make accurate assumptions about where code lives.

## Overview

- **Type:** Native C++ Windows game, Direct3D 12, built with MSBuild (Visual Studio C++ project).
- **Solution:** `VehicleGame.slnx` (XML solution format) at the repository root. All project paths in it are relative to the folder containing this file, e.g. `VehicleGame/VehicleGame.vcxproj` (project file at `VehicleGame\VehicleGame\VehicleGame.vcxproj`). Resolve project paths against the repository root, not the nested `VehicleGame\` project folder.
- **Platform:** Windows x64 (Debug and Release configurations). Targets the Microsoft GDK (Xbox PC / Microsoft Store).
- **Package management:** vcpkg in manifest mode.
- **Language standard:** Modern C++ (C++17 or later). Precompiled headers are used.

## Repository Layout

```
VehicleGame/                      <- Repository / solution root
├── VehicleGame/                  <- Main C++ project (all source lives here)
│   ├── VehicleGame.vcxproj       <- MSBuild project file (add new source files here)
│   ├── VehicleGame.vcxproj.filters
│   ├── vcpkg.json                <- vcpkg manifest (dependencies)
│   ├── vcpkg-configuration.json  <- vcpkg registry configuration
│   ├── README.md
│   ├── Main.cpp                  <- Entry point, Win32 window procedure
│   ├── Game.h / Game.cpp         <- Top-level Game class (update/render loop)
│   ├── DeviceResources.h / .cpp  <- D3D12 device, swap chain, command queue
│   ├── StepTimer.h               <- Fixed/variable timestep helper
│   ├── pch.h / pch.cpp           <- Precompiled header
│   ├── resource.rc               <- Win32 resources
│   ├── MicrosoftGameConfig.mgc   <- GDK game configuration
│   ├── Engine/                   <- Game engine/state framework
│   │   ├── State/                <- Engine state, modes base, input system
│   │   │   ├── EngineState.h     <- Global engine state container
│   │   │   ├── EngineMode.h      <- Abstract mode interface
│   │   │   ├── CoreMode.h        <- Shared base for modes
│   │   │   ├── ModeTimer.h       <- Timers tied to a mode
│   │   │   ├── Input.h           <- Input abstraction
│   │   │   ├── InputDevices.h    <- Device-specific input
│   │   │   ├── InputFrame.h      <- Per-frame input snapshot
│   │   │   └── InputSystem.h     <- Input polling/dispatch
│   │   └── Modes/                <- Concrete game modes/screens
│   │       ├── LoadingMode.h
│   │       ├── MenuMode.h
│   │       └── PlayMode.h
│   └── (x64/)                    <- Build output (ignore; never edit)
└── x64/Debug/                    <- Build output and copied runtime DLLs/assets (ignore)
```

## Where Things Go

| Kind of change | Location |
|---|---|
| Application startup / window handling | `VehicleGame/Main.cpp` |
| Game loop, update and render logic | `VehicleGame/Game.h`, `VehicleGame/Game.cpp` |
| Direct3D 12 setup, swap chain, resources | `VehicleGame/DeviceResources.h/.cpp` |
| New game mode or screen | `VehicleGame/Engine/Modes/<Name>Mode.h` (derive from `CoreMode`/`EngineMode`) |
| Mode-independent engine state | `VehicleGame/Engine/State/` |
| Input handling | `VehicleGame/Engine/State/Input*.h` |
| Timing utilities | `VehicleGame/StepTimer.h` or `VehicleGame/Engine/State/ModeTimer.h` |
| Precompiled header includes | `VehicleGame/pch.h` |
| Dependencies (libraries, SDKs) | `VehicleGame/vcpkg.json` |
| Win32 resources, icons, manifests | `VehicleGame/resource.rc`, `MicrosoftGameConfig.mgc` |
| Shaders (if added) | `VehicleGame/` alongside other sources, registered in `.vcxproj` as HLSL |
| Image assets | Under an `Assets/` folder next to the project, copied on build |

## Conventions

- Headers and sources are flat in `VehicleGame/` for core framework files; engine code is grouped under `Engine/`.
- Header-only helpers (e.g. `StepTimer.h`, engine `.h` files) are common; a `.cpp` is only added when implementation is non-trivial.
- Every new `.cpp` must be added to `VehicleGame.vcxproj` (and to `.vcxproj.filters` to match the folder structure).
- Every new `.cpp` must include `pch.h` first.
- Namespaces and class names use PascalCase; member variables are typically prefixed with `m_`.
- Mode classes derive from the engine mode base and are registered with the engine state rather than instantiated ad hoc.

## Build and Dependencies

- Build with Visual Studio or `msbuild VehicleGame\VehicleGame.vcxproj /p:Configuration=Debug /p:Platform=x64`.
- Add dependencies by editing `VehicleGame/vcpkg.json`; vcpkg restores them during build.
- PlayFab support is optional and enabled through the `ms-gdk` feature `playfab` (see README); it requires uncommenting includes in `pch.h`.
- Do not modify files under `x64/` or any `*.obj`, `*.pdb`, `*.log`, or `*.dll` output.

## Planning Guidance

- When planning a feature, assume the main logic lives in `VehicleGame/` and engine-level changes go in `VehicleGame/Engine/`.
- Plan changes to new files as: create the file, add it to `VehicleGame.vcxproj`, add it to `VehicleGame.vcxproj.filters`, then update includes.
- Gameplay or screen work usually touches: a new mode in `Engine/Modes/`, registration in engine state, and `Game.cpp` for loop integration.
- Rendering work usually touches `DeviceResources` and `Game.cpp`.
- Input work usually touches `Engine/State/Input*.h`.
