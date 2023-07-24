#include "Application.h"

#include "Game.h"
#include "Engine/Entities/OasisDirt.h"
#include "Engine/Entities/OasisPalmAndGrass.h"
#include "Engine/Entities/OasisRocks.h"
#include "Engine/Entities/OasisSand.h"
#include "Engine/Entities/OasisWater.h"
#include "Engine/Entities/Box.h"
#include "Engine/Entities/Monkey.h"
#include "Engine/Entities/PingleLogo.h"
#include "Engine/Entities/WilliamSphere.h"
#include "Engine/Renderer/DirectX12.h"
#include "Engine/Utils/ColorHelper.h"
#include "Engine/Utils/Logger.h"
#include "ImGUI/imgui_impl_dx12.h"
#include "ImGUI/misc/cpp/imgui_stdlib.h"
#include "World/World.h"
#include "EngineHeaders.h"

LRESULT Window::WndProc(HWND hwnd, unsigned int msg, WPARAM wParam, LPARAM lParam)
{
	if (ImGui_ImplWin32_WndProcHandler(hwnd, msg, wParam, lParam))
	{
		return true;
	}
	GameWndProcHandler(hwnd, msg, wParam, lParam);
	static bool bCatch = false;
	switch (msg)
	{
	case WM_KEYDOWN:
		if (wParam == VK_ESCAPE) {
			if (MessageBox(0, L"Are you sure you want to exit?",
			               L"Really?", MB_YESNO | MB_ICONQUESTION) == IDYES)
			{
				//Running = false;
				DestroyWindow(hwnd);
			}
		}
		if (wParam == VK_F1) {
			bCatch = !bCatch;
		}
		return 0;
	

	case WM_DESTROY: // x button on top right corner of window was pressed
		//Running = false;
		PostQuitMessage(0);
		return 0;

	case  WM_EXITSIZEMOVE:
		Window::GetInstance().Resize();
		return 0;
	case WM_SIZE:
		{
			if(wParam == SIZE_MAXIMIZED)
			{
				Window::GetInstance().Resize();
				return 0;
			}
			if(wParam == SIZE_RESTORED)
			{
				Window::GetInstance().Resize();
				return 0;
			}
		}
		
	}
	return DefWindowProc(hwnd,
	                     msg,
	                     wParam,
	                     lParam);
}

Window& Window::GetInstance()
{
	static Window _instance;
	return _instance;
}

void Window::Resize()
{
	auto& DX = DirectX12::GetInstance();
	RECT clientRect = {};
	::GetClientRect(hwnd, &clientRect);

	Width = clientRect.right - clientRect.left;
	Height = clientRect.bottom - clientRect.top;
	CachedHeight = Height;
	CachedWidth = Width;
	if (DX.IsInited())
	{
		DX.NeedsReize = true; //Todo: Make it better
	}
}

bool Window::InitializeWindow(HINSTANCE hInstance, int ShowWnd, bool Fullscreen)
{
	//TODO: IMPLEMENT FULLSCREEN
	//if (fullscreen)
	//{
	//	HMONITOR hmon = MonitorFromWindow(DX.hwnd,
	//		MONITOR_DEFAULTTONEAREST);
	//	MONITORINFO mi = { sizeof(mi) };
	//	GetMonitorInfo(hmon, &mi);
	//
	//	Width = mi.rcMonitor.right - mi.rcMonitor.left;
	//	Height = mi.rcMonitor.bottom - mi.rcMonitor.top;
	//}

	WNDCLASSEX wc;

	wc.cbSize = sizeof(WNDCLASSEX);
	wc.style = CS_HREDRAW | CS_VREDRAW;
	wc.lpfnWndProc = WndProc;
	wc.cbClsExtra = NULL;
	wc.cbWndExtra = NULL;
	wc.hInstance = hInstance;
	wc.hIcon = LoadIcon(NULL, IDI_APPLICATION);
	wc.hCursor = LoadCursor(NULL, IDC_ARROW);
	wc.hbrBackground = (HBRUSH)(COLOR_WINDOW + 2);
	wc.lpszMenuName = NULL;
	wc.lpszClassName = L"Cringine";
	wc.hIconSm = LoadIcon(NULL, IDI_APPLICATION);

	if (!RegisterClassEx(&wc))
	{
		MessageBox(NULL, L"Error registering class",
		           L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	Window::GetInstance().hwnd = CreateWindowEx(NULL,
	                                            L"Cringine",
	                                            L"Cringine",
	                                            WS_OVERLAPPEDWINDOW,
	                                            CW_USEDEFAULT, CW_USEDEFAULT,
	                                            Window::GetInstance().Width, Window::GetInstance().Height,
	                                            NULL,
	                                            NULL,
	                                            hInstance,
	                                            NULL);

	if (!Window::GetInstance().hwnd)
	{
		MessageBox(NULL, L"Error creating window",
		           L"Error", MB_OK | MB_ICONERROR);
		return false;
	}

	if (Fullscreen)
	{
		SetWindowLong(Window::GetInstance().hwnd, GWL_STYLE, 0);
	}

	ShowWindow(Window::GetInstance().hwnd, ShowWnd);
	UpdateWindow(Window::GetInstance().hwnd);

	return true;
}

void Application::LoadContent()
{
	logger.Log(INFO,"Loading Content...");
	CurrentWorld = new World();
	
	EntityManager& EManager = CurrentWorld->GetEntityManager();
	//EManager.ADD_ENTITY(std::make_shared(&Game::GetInstance().Camera));
	//EManager.ADD_ENTITY(std::make_shared<WilliamSphere>());
	//
	//EManager.ADD_ENTITY(std::make_shared<Box>());
	//EManager.ADD_ENTITY(std::make_shared<PingleLogo>());
	////Lines
	//{
	//	//X
	//	auto linex = std::make_shared<Box>();
	//	linex->m_Transform.Scale = XMMatrixScaling(999.f, 0.01f, 0.01f);
	//	EManager.ADD_ENTITY(linex);
	//	//Y
	//	auto liney = std::make_shared<Box>();
	//	liney->m_Transform.Scale = XMMatrixScaling(0.01f, 999.f, 0.01f);
	//	EManager.ADD_ENTITY(liney);
	//	//Z
	//	auto linez = std::make_shared<Box>();
	//	linez->m_Transform.Scale = XMMatrixScaling(0.01f, 0.01f, 999.0f);
	//	EManager.ADD_ENTITY(linez);
	//}
	//
	//EManager.ADD_ENTITY(std::make_shared<PingleLogo>());
	//
	//EManager.ADD_ENTITY(std::make_shared<PingleLogo>());
	//
	//EManager.ADD_ENTITY(std::make_shared<PingleLogo>());
	//
	//EManager.ADD_ENTITY(std::make_shared<Monkey>());
	//
	//{
		auto obj = std::make_shared<OasisDirt>();
		obj->GetTransform().SetPosition(XMFLOAT4(1.f, -1.f, 20.0f, 0.0f));
		EManager.ADD_ENTITY(obj);
	//}
	//{
	//	auto obj = std::make_shared<OasisPalmAndGrass>();
	//	obj->GetTransform().SetPosition(XMFLOAT4(1.f, -1.f, 20.0f, 0.0f));
	//	EManager.ADD_ENTITY(obj);
	//}
	//{
	//	auto obj = std::make_shared<OasisRocks>();
	//	obj->GetTransform().SetPosition(XMFLOAT4(1.f, -1.f, 20.0f, 0.0f));
	//	EManager.ADD_ENTITY(obj);
	//}
	//{
	//	auto obj = std::make_shared<OasisSand>();
	//	obj->GetTransform().SetPosition(XMFLOAT4(1.f, -1.f, 20.0f, 0.0f));
	//	EManager.ADD_ENTITY(obj);
	//}
	//{
	//	auto obj = std::make_shared<OasisWater>();
	//	obj->GetTransform().SetPosition(XMFLOAT4(1.f, -1.f, 20.0f, 0.0f));
	//	EManager.ADD_ENTITY(obj);
	//}
	//EManager.ADD_ENTITY(new OasisWater(XMFLOAT4(1.f, -1.f, 20.0f, 0.0f)));
	//

	logger.Log(INFO,"Finished loading content...");
}

ImGUIContext& ImGUIContext::GetInstance()
{
	static ImGUIContext _instance;
	return _instance;
}

void ImGUIContext::InitIMGUIDX()
{
	D3D12_DESCRIPTOR_HEAP_DESC desc = {};
	desc.Type = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	desc.NumDescriptors = 2;
	desc.Flags = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	auto hr = DirectX12::GetInstance().m_pDevice->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&DirectX12::GetInstance().m_pDescriptorHeap[DirectX12::HEAP::SRV]));
	if(FAILED(hr))
	{
		logger.Log(ERR, "Cannot create SRV descriptor heap!");
	}
}

void ImGUIContext::InitIMGUI()
{
	const auto& DX = DirectX12::GetInstance();

	ImGui::CreateContext();
	
	ImGui_ImplWin32_Init(Window::GetInstance().hwnd);
	ImGui_ImplDX12_Init(DX.m_pDevice, DX.frameBufferCount,
		DXGI_FORMAT_R8G8B8A8_UNORM, DX.m_pDescriptorHeap[DirectX12::HEAP::SRV],
		DX.m_pDescriptorHeap[DirectX12::HEAP::SRV]->GetCPUDescriptorHandleForHeapStart(),
		DX.m_pDescriptorHeap[DirectX12::HEAP::SRV]->GetGPUDescriptorHandleForHeapStart());
}
void DrawParams(const std::shared_ptr<IPropertyManager>& ClassWithProperties)
{
	auto Properties = ClassWithProperties->GetAllProperties();
	for (auto Property : Properties)
	{
		if (Property.second.type() == typeid(Transform*))
		{
			auto Converted = std::any_cast<Transform*>(Property.second);
			
			ImGui::InputFloat3("Position", reinterpret_cast<float*>(&Converted->Position));
			//float scale[3];
			//scale[0] = Converted->Scale.r[0].m128_f32[0];
			//scale[1] = Converted->Scale.r[1].m128_f32[1];
			//scale[2] = Converted->Scale.r[2].m128_f32[2];
			//
			//if(ImGui::InputFloat3("Scale", reinterpret_cast<float*>(&scale)))
			//{
			//	Converted->SetScale(XMMatrixScaling(scale[0], scale[1], scale[2]));
			//}


			float m13 = Converted->RotMat._13;
			float m21 = Converted->RotMat._21;
			float m22 = Converted->RotMat._22;
			float m23 = Converted->RotMat._23;
			float m33 = Converted->RotMat._33;
			float rotation[3] = {};

			// Calculate pitch (rotation around x-axis)
			rotation[0] = XMConvertToDegrees(std::asin(-m23));

			// Calculate yaw (rotation around y-axis)
			if (m33)
			{
				rotation[1] = XMConvertToDegrees(std::atan2(m13, m33));
			}
			// Calculate roll (rotation around z-axis)
			
			if (m22)
			{
				rotation[2] = XMConvertToDegrees(std::atan2(m21, m22));
			}

			if (ImGui::InputFloat3("Rotation", reinterpret_cast<float*>(&rotation), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				Converted->SetRotation(rotation[0], rotation[1], rotation[2]);
			}
		}
		if (Property.second.type() == typeid(std::string*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<std::string*>(Property.second);
			ImGui::InputText(Property.first.c_str(), Converted);
		}
		if (Property.second.type() == typeid(float*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<float*>(Property.second);
			ImGui::InputFloat(Property.first.c_str(), Converted);
		}
		if (Property.second.type() == typeid(bool*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<bool*>(Property.second);
			ImGui::Checkbox(Property.first.c_str(), Converted);
		}
		if (Property.second.type() == typeid(ColorHelper::RGBA*))
		{
			auto Converted = std::any_cast<ColorHelper::RGBA*>(Property.second);
			ImGui::ColorEdit4(Property.first.c_str(), (float*)Converted);
		}
	}

}
void DrawParams(IPropertyManager* ClassWithProperties)
{
	auto Properties = ClassWithProperties->GetAllProperties();
	for (auto Property : Properties)
	{
		if (Property.second.type() == typeid(Transform*))
		{
			auto Converted = std::any_cast<Transform*>(Property.second);

			ImGui::InputFloat3("Position", reinterpret_cast<float*>(&Converted->Position));
			//float scale[3];
			//scale[0] = Converted->Scale.r[0].m128_f32[0];
			//scale[1] = Converted->Scale.r[1].m128_f32[1];
			//scale[2] = Converted->Scale.r[2].m128_f32[2];
			//
			//if(ImGui::InputFloat3("Scale", reinterpret_cast<float*>(&scale)))
			//{
			//	Converted->SetScale(XMMatrixScaling(scale[0], scale[1], scale[2]));
			//}


			float m13 = Converted->RotMat._13;
			float m21 = Converted->RotMat._21;
			float m22 = Converted->RotMat._22;
			float m23 = Converted->RotMat._23;
			float m33 = Converted->RotMat._33;
			float rotation[3] = {};

			// Calculate pitch (rotation around x-axis)
			rotation[0] = XMConvertToDegrees(std::asin(-m23));

			// Calculate yaw (rotation around y-axis)
			if (m33)
			{
				rotation[1] = XMConvertToDegrees(std::atan2(m13, m33));
			}
			// Calculate roll (rotation around z-axis)

			if (m22)
			{
				rotation[2] = XMConvertToDegrees(std::atan2(m21, m22));
			}

			if (ImGui::InputFloat3("Rotation", reinterpret_cast<float*>(&rotation), "%.3f", ImGuiInputTextFlags_EnterReturnsTrue))
			{
				Converted->SetRotation(rotation[0], rotation[1], rotation[2]);
			}
		}
		if (Property.second.type() == typeid(std::string*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<std::string*>(Property.second);
			ImGui::InputText(Property.first.c_str(), Converted);
		}
		if (Property.second.type() == typeid(DirectX::XMFLOAT3*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<DirectX::XMFLOAT3*>(Property.second);
			ImGui::InputFloat3(Property.first.c_str(), reinterpret_cast<float*>(Converted));
		}
		if (Property.second.type() == typeid(float*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<float*>(Property.second);
			ImGui::InputFloat(Property.first.c_str(), Converted);
		}
		if (Property.second.type() == typeid(bool*))
		{
			//ImGui::Text(Property.first.c_str());
			auto Converted = std::any_cast<bool*>(Property.second);
			ImGui::Checkbox(Property.first.c_str(), Converted);
		}
		if (Property.second.type() == typeid(ColorHelper::RGBA*))
		{
			auto Converted = std::any_cast<ColorHelper::RGBA*>(Property.second);
			ImGui::ColorEdit4(Property.first.c_str(), (float*)Converted);
		}
	}

}
static ImGuiTreeNodeFlags base_flags = ImGuiTreeNodeFlags_OpenOnArrow | ImGuiTreeNodeFlags_OpenOnDoubleClick | ImGuiTreeNodeFlags_SpanAvailWidth;

void RenderComponentAndSubComponents(std::shared_ptr<BaseComponent>& component, bool& isSelected)
{
	auto & SubComponents = component->GetSubComponents();
	auto nSubComponents = SubComponents.size();
	bool isOpen = ImGui::TreeNodeEx(component.get(), base_flags, "%s",component->m_Name.c_str());
	bool isClicked = ImGui::IsItemClicked();

	if (isClicked && !isSelected)
	{
		ImGUIContext::GetInstance().SelectedProperty = component;
		ImGUIContext::GetInstance().IsRootSelected = false;
		isSelected = true;
	}

	if (isOpen)
	{
		for (unsigned long nSubComponent = 0; nSubComponent < nSubComponents; ++nSubComponent)
		{
			auto SubComponent = SubComponents[nSubComponent];
			RenderComponentAndSubComponents(SubComponent, isSelected);
		}

		ImGui::TreePop();
	}
}
void DisplayStringCombo(const char* label, const std::vector<std::string>& strings, int& selectedItemIndex)
{
	// Ensure selectedItemIndex is within valid range
	selectedItemIndex = std::clamp(selectedItemIndex, 0, static_cast<int>(strings.size()) - 1);

	// Convert vector of std::strings to vector of const char*
	std::vector<const char*> stringItems;
	stringItems.reserve(strings.size());
	for (const auto& str : strings)
	{
		stringItems.push_back(str.c_str());
	}

	// Display the combo box
	if (ImGui::BeginCombo(label, stringItems[selectedItemIndex]))
	{
		for (int i = 0; i < stringItems.size(); ++i)
		{
			bool isItemSelected = (selectedItemIndex == i);
			ImGui::PushID(stringItems[i]);
			if (ImGui::Selectable(stringItems[i], isItemSelected))
			{
				selectedItemIndex = i;
			}
			ImGui::PopID();
			if (isItemSelected)
			{
				ImGui::SetItemDefaultFocus();
			}
		}
		ImGui::EndCombo();
	}
}
void ImGUIContext::RenderIMGUI()
{
	const auto& DX = DirectX12::GetInstance();

	ImGui_ImplDX12_NewFrame();
	ImGui_ImplWin32_NewFrame();
	ImGui::NewFrame();
	if (ImGui::IsKeyPressed(ImGuiKey_Delete))
	{
		if (auto Property = ImGUIContext::GetInstance().SelectedProperty.lock())
		{
			if (auto EntityPtr = std::dynamic_pointer_cast<Entity>(Property))
			{
				if (auto World = Application::GetInstance().CurrentWorld)
				{
					World->GetEntityManager().RemoveEntity(EntityPtr);
					SelectedProperty.reset();
				}
			}

		}
	}
	//ImGui::ShowDemoWindow();
	if (ImGui::BeginMainMenuBar())
	{
		if (ImGui::BeginMenu("World"))
		{
			if (ImGui::MenuItem("New", "CTRL+N")) {}
			if (ImGui::MenuItem("Open", "CTRL+O"))
			{
				Application::GetInstance().CurrentWorld->Load("Test");
			}
			if (ImGui::MenuItem("Save", "CTRL+S"))
			{
				Application::GetInstance().CurrentWorld->Save("Test");
			}
			if (ImGui::MenuItem("Exit", "CTRL+ESCAPE")) {}
			ImGui::EndMenu();
		}
		ImGui::EndMainMenuBar();
	}
	ImGui::Begin("EntityList");
	ImGui::SetNextItemOpen(true);
	ImGui::BeginChild("Entity Box", { 0, 200 }, false, ImGuiWindowFlags_HorizontalScrollbar);
	if (ImGui::TreeNodeEx("Entities", ImGuiTreeNodeFlags_SpanAvailWidth))
	{
		bool isClicked = ImGui::IsItemClicked();

		if (isClicked )
		{
			SelectedProperty.reset();
			IsRootSelected = true;
		}

		if (auto World = Application::GetInstance().CurrentWorld)
		{
			auto Entities = World->GetEntityManager().GetEntites();
			for (auto Entity : Entities)
			{
				bool isSelected = false; // Flag to track if the item is selected
				ImGui::PushID(Entity.get());
				if (ImGui::Selectable(Entity->GetName().c_str(), isSelected))
				{
					ImGUIContext::GetInstance().SelectedProperty = Entity;
					ImGUIContext::GetInstance().IsRootSelected = false;

					// Item is selected, handle the selection logic here
					// For example, you can store the selected entity or perform some action
					// based on the selected entity.
				}
				ImGui::PopID();
				auto& Components = Entity->GetComponentManager().GetAllComponents();
				unsigned long nComponents = Components.size();
				for (unsigned long nComponent = 0; nComponent < nComponents; ++nComponent)
				{
					auto Component = Components[nComponent];
					isSelected = false;
					RenderComponentAndSubComponents(Component, isSelected);
				}
			}
		}
		ImGui::TreePop();
	}
	ImGui::EndChild();

	ImGui::Separator();
	if (!ImGUIContext::GetInstance().IsRootSelected && !ImGUIContext::GetInstance().SelectedProperty.expired())
	{
		ImGui::Text("Parameters");
		if (const auto pSelectedProperty = ImGUIContext::GetInstance().SelectedProperty.lock())
		{
			DrawParams(pSelectedProperty);
		}
	}

	ImGui::Separator();

	if(ImGUIContext::GetInstance().IsRootSelected)
	{
		const auto& RegisteredEntityTypes = EntityManager::GetRegisteredEntitiesTypes();
		static int Selected = 0;
		DisplayStringCombo("Entity", RegisteredEntityTypes, Selected);
		ImGui::SameLine();
		if(ImGui::Button("Create"))
		{
			Application::GetInstance().CurrentWorld->GetEntityManager().ADD_ENTITY(EntityManager::CreateEntitiy(RegisteredEntityTypes[Selected].c_str()));
		}
	}
	else
	{
		if (auto Prop = SelectedProperty.lock())
		{
			if (auto SelectedEntity = std::dynamic_pointer_cast<Entity>(Prop))
			{
				if (ImGui::Button("Delete"))
				{
					Application::GetInstance().CurrentWorld->GetEntityManager().RemoveEntity(SelectedEntity);
				}
			}
		}
	}
	ImGui::Text("Misc");
	ImGui::Separator();
	if(auto obj = SelectedProperty.lock())
	{
		ImGui::Text("Pointer references = %i", obj.use_count());
	}
	
	ImGui::End();
	const auto& Cam = Game::GetInstance().Camera;
	{
		ImGui::Begin("Info", nullptr, ImGuiWindowFlags_AlwaysAutoResize);
		ImGui::Text("Press F1 to toggle mouse input!");
		if(ImGui::Button(Game::GetInstance().bPaused ? "Continue" : "Pause"))
		{
			 Game::GetInstance().bPaused = !Game::GetInstance().bPaused;
		}
		//ImGui::Checkbox(Game::GetInstance().bPaused ? "Continue" : "Pause", &Game::GetInstance().bPaused);
		ImGui::End();

		ImGui::Begin("Camera",nullptr,ImGuiWindowFlags_AlwaysAutoResize );
		//ImGui::Text("Camera: X=%f Y=%f Z=%f", Cam.m_Transform.GetPosition().x,
		//	Cam.m_Transform.GetPosition().y,
		//	Cam.m_Transform.GetPosition().z);
		ImGui::Text("Camera: Pitch=%f Yaw=%f", Cam.camPitch,
			Cam.camYaw);
		{
			auto camx = XMVectorGetX(Cam.cameraTarget);
			auto camy = XMVectorGetY(Cam.cameraTarget);
			auto camz = XMVectorGetZ(Cam.cameraTarget);
			ImGui::Text("Camera Target: X=%f Y=%f Z=%f", camx,camy,camz);
		}
		{
			auto camx = XMVectorGetX(Cam.camForward);
			auto camy = XMVectorGetY(Cam.camForward);
			auto camz = XMVectorGetZ(Cam.camForward);
			ImGui::Text("Camera Forward: X=%f Y=%f Z=%f", camx, camy, camz);
		}
		if (ImGui::InputFloat("FOV", &Game::GetInstance().Camera.m_FOV))
		{
			Game::GetInstance().Camera.SetFOV(Game::GetInstance().Camera.m_FOV);
		}
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	
		ColorHelper::RGBA RGB = ColorHelper::HSV2RGB(Game::GetInstance().BGColorHSV);
		ImGui::ColorPicker3("Clear",(float*)&RGB);

		Game::GetInstance().BGColorHSV = ColorHelper::RGB2HSV(RGB);
		ImGui::Text("Light");
	
		DrawParams(&Game::GetInstance().light);
		
		ImGui::End();
	}

	// Rendering
	ImGui::Render();
	
	DX.m_pImGUICommandList->SetDescriptorHeaps(1, &DX.m_pDescriptorHeap[DirectX12::HEAP::SRV]);

	ImGui_ImplDX12_RenderDrawData(ImGui::GetDrawData(), DX.m_pImGUICommandList);
}
