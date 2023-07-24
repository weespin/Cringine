#include <chrono>

#include "Application.h"
#include "Game.h"
#include "stdafx.h"
#include "Engine/Input/InputSystem.h"
#include "Engine/Renderer/DirectX12.h"
#include "ImGUI/imgui.h"
#include "shellscalingapi.h"
#include "World/World.h"
//#include "PropertyManager.h"
struct Entity;

void Update();
void MainLoop();
//class Test : public IPropertyManager
//{
//	CREATE_PROPERTY(XMFLOAT4X4, TestAAA, {});
//};
int WINAPI WinMain(HINSTANCE hInstance,    //Main windows function
	HINSTANCE hPrevInstance,
	LPSTR lpCmdLine,
	int nShowCmd)

{;
//Test t;
//	auto a =t.GetProperty<XMFLOAT4X4>("TestAAA");
	SetProcessDpiAwareness(PROCESS_PER_MONITOR_DPI_AWARE);
	Window::GetInstance().InitializeWindow(hInstance, nShowCmd, false /* TODO: FullScreen!!! */);
	// create the window
	DirectX12::GetInstance().InitD3D();
	ImGUIContext::GetInstance().InitIMGUIDX();
	ImGUIContext::GetInstance().InitIMGUI();
	Application::GetInstance().LoadContent();
	// start the main loop
	MainLoop();

	// we want to wait for the gpu to finish executing the command list before we start releasing everything
	DirectX12::GetInstance().Shutdown();

	return 0;
}

void MainLoop()
{
	MSG msg;
	ZeroMemory(&msg, sizeof(MSG));

	while (Application::GetInstance().Running)
	{
		if (PeekMessage(&msg, NULL, 0, 0, PM_REMOVE))
		{
			if (msg.message == WM_QUIT)
				break;

			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		else
		{
			Update();
		}
	}
}


void Update()
{
	// Define a clock and track elapsed time since the last update
	static std::chrono::high_resolution_clock::time_point t0 = std::chrono::high_resolution_clock::now();

	// Calculate the delta time
	auto t1 = std::chrono::high_resolution_clock::now();
	float deltaTime = std::chrono::duration<float>(t1 - t0).count();
	t0 = t1;

	// Update the camera
	Game::GetInstance().Camera.Update(deltaTime);

	// Accumulate the elapsed time
	static float accumulatedTime = 0.0f;
	accumulatedTime += deltaTime;

	// Check if it's time for a game update (approximately every 16ms)
	constexpr float updateInterval = 0.016f;
	while (accumulatedTime >= updateInterval)
	{
		if (auto* World = Application::GetInstance().CurrentWorld)
		{
			World->Tick(updateInterval);
		}

		accumulatedTime = 0;
	}
	if (auto* World = Application::GetInstance().CurrentWorld)
	{
		World->PrepareRender();
	}
	DirectX12::GetInstance().Render(); // execute the command queue (rendering the scene is the result of the GPU executing the command lists)

	// Update input hold
	InputSystem::GetInstance().UpdateHold();
	if(DirectX12::GetInstance().NeedsReize)
	{
		DirectX12::GetInstance().Resize(Window::GetInstance().Width, Window::GetInstance().Height);
	}
}