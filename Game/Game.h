//
// Game.h
//

#pragma once

#include "DeviceResources.h"
#include "StepTimer.h"
#include "Engine/State/CoreMode.h"
#include "Engine/State/InputDevices.h"
#include "Engine/State/InputSystem.h"

#include <memory>


// A basic game implementation that creates a D3D12 device and
// provides a game loop.
class Game final : public DX::IDeviceNotify
{
public:

    Game() noexcept(false);
    ~Game();

    Game(Game&&) = default;
    Game& operator= (Game&&) = default;

    Game(Game const&) = delete;
    Game& operator= (Game const&) = delete;

    // Initialization and management
    void Initialize(HWND window, int width, int height);

    // Basic game loop
    void Tick();

    // Input
    void OnRawInput(LPARAM lParam);
    void OnChar(char16_t unit);

    // IDeviceNotify
    void OnDeviceLost() override;
    void OnDeviceRestored() override;

    // Messages
    void OnActivated();
    void OnDeactivated();
    void OnSuspending();
    void OnResuming();
    void OnWindowMoved();
    void OnDisplayChange();
    void OnWindowSizeChanged(int width, int height);

    // Properties
    void GetDefaultSize( int& width, int& height ) const noexcept;

private:

    void PollInput();
    void Update(double elapsedSeconds);
    void Render();

    void Clear();

    void CreateDeviceDependentResources();
    void CreateWindowSizeDependentResources();

    // Device resources.
    std::unique_ptr<DX::DeviceResources>        m_deviceResources;

    // Rendering loop timer.
    DX::StepTimer                               m_timer;

    // Engine core state machine.
    Engine::CoreMode                            m_core;
    double                                      m_accumulator = 0.0;

    // Polled input devices and absolute input state.
    Engine::InputDevices                        m_devices;
    Engine::InputSystem                         m_input;
    Engine::RawKeyboardFrame                    m_keyboard;

    // If using the DirectX Tool Kit for DX12, uncomment this line:
    // std::unique_ptr<DirectX::GraphicsMemory> m_graphicsMemory;
};
