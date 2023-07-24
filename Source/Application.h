#pragma once
#include <d3d12.h>
#include <DirectXMath.h>
#include <memory>
#include <vcruntime_typeinfo.h>
#include <vector>

#include "Engine/Utils/Logger.h"
#include "ImGUI/imgui_impl_win32.h"

class IPropertyManager;
class World;
struct Entity;
using namespace DirectX; // we will be using the directxmath library



extern IMGUI_IMPL_API LRESULT ImGui_ImplWin32_WndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);
extern IMGUI_IMPL_API LRESULT GameWndProcHandler(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

#define ADD_ENTITY(a) AddEntity(a, typeid(a).name())


class Window
{
public:
	static LRESULT CALLBACK WndProc(HWND hwnd,
	UINT msg,
	WPARAM wParam,
	LPARAM lParam);

	static Window& GetInstance();
	void Resize();
	bool InitializeWindow(HINSTANCE hInstance, int ShowWnd, bool Fullscreen);

	int Width = 800;
	int Height = 600;
	int CachedHeight;
	int CachedWidth;

	bool bUseVSYNC = false;
	bool FullScreen = false;
	HWND hwnd = NULL;
};
class Application
{
public:
	static Application& GetInstance()
	{
		static Application _instance;
		return _instance;
	}
	bool Running = true;
	void LoadContent();
	World* CurrentWorld = nullptr;
};

class ImGUIContext
{
public:
	static ImGUIContext& GetInstance();
	void InitIMGUIDX();

	void InitIMGUI();
	void RenderIMGUI();
	std::weak_ptr<IPropertyManager> SelectedProperty;
	bool IsRootSelected;
};