#include "InputSystem.h"

#include <windowsx.h>
#include "Application.h"
#include "Engine/Renderer/DirectX12.h"

void InputSystem::ProcessInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	
	static bool bCatch = false;

	switch(msg)
	{

	case WM_KEYDOWN: 
		{
			if(ImGui::GetIO().WantCaptureKeyboard)
			{
				return;
			}
			m_holdedKeys.erase(std::remove(m_holdedKeys.begin(), m_holdedKeys.end(), wParam), m_holdedKeys.end());
			for (const auto& [EventId, Event] : m_keyDownEvents)
			{
				Event(static_cast<char>(wParam));
			}
			m_holdedKeys.push_back(static_cast<char>(wParam));
			if (wParam == VK_F1)
			{
				if(!bCatch)
				{
					ShowCursor(false);
				}
				else
				{
					ShowCursor(true);
					
				}
				bCatch = !bCatch;
			}
			if(wParam == VK_F11)
			{
				
				Window::GetInstance().FullScreen = !Window::GetInstance().FullScreen;
				if(Window::GetInstance().FullScreen)
				{
					int monitorx = GetSystemMetrics(SM_CXSCREEN);
					int monitory = GetSystemMetrics(SM_CYSCREEN);
					Window::GetInstance().CachedHeight = Window::GetInstance().Height;
					Window::GetInstance().CachedWidth = Window::GetInstance().Width;

					Window::GetInstance().Height = monitory;
					Window::GetInstance().Width = monitorx;
				}
				else
				{
					RECT r = {};
					GetWindowRect(Window::GetInstance().hwnd, &r);
				
					
					Window::GetInstance().Height = Window::GetInstance().CachedHeight;
					Window::GetInstance().Width = Window::GetInstance().CachedWidth;
				}
				DirectX12::GetInstance().NeedsReize = true;
			}
			break;
		}
	case WM_KEYUP: 
		{
			for (const auto& [EventId, Event] : m_keyUpEvents)
			{
				Event(static_cast<char>(wParam));
			}
			m_holdedKeys.erase(std::remove(m_holdedKeys.begin(), m_holdedKeys.end(), static_cast<char>(wParam)), m_holdedKeys.end());
			break;
		}
	case WM_MOUSEMOVE:
		{
			const int xPos = GET_X_LPARAM(lParam);
			const int yPos = GET_Y_LPARAM(lParam);
			//logger.Log(DEBUG,"Mouse X = %i Y = %i",xPos,yPos);
			if (bCatch)
			{
				RECT r = {};
				GetWindowRect(Window::GetInstance().hwnd, &r);

				RECT p = {};
				MapWindowPoints(Window::GetInstance().hwnd, GetParent(Window::GetInstance().hwnd), (LPPOINT)&p, 2);

				const int centerX = (r.right - r.left) / 2;
				const int centerY = (r.bottom - r.top) / 2;

				const int dx = centerX - xPos;
				const int dy = centerY - yPos;
				//logger.Log(DEBUG, "Mouse DX = %i DY = %i ", dx, dy);

				SetCursorPos(centerX + p.left, centerY + p.top);

				for (const auto& [EventId, Event] : m_mouseMoveEvents)
				{
					Event(dx, dy);
				}
			}
		}
	default:;
	}
}

void InputSystem::UpdateHold()
{
	for (const auto& key : m_holdedKeys)
	{
		for(auto& event : m_keyHoldEvents)
		{
			event.second(key);
		}
	}
}

EventId InputSystem::RegisterOnKeyDown(const OnKeyEvent& event)
{
	const EventId newEventId = GetNextEventId();
	m_keyDownEvents[newEventId] = event;
	return  newEventId;
}

EventId InputSystem::RegisterOnKeyUp(const OnKeyEvent& event)
{
	const EventId newEventId = GetNextEventId();

	m_keyUpEvents[newEventId] = event;
	return  newEventId;

}

EventId InputSystem::RegisterOnKeyHold(const OnKeyEvent& event)
{
	const EventId newEventId = GetNextEventId();

	m_keyHoldEvents[newEventId] = event;
	return  newEventId;

}

EventId InputSystem::RegisterOnMouseMove(const OnMouseMoveEvent& event)
{
	const EventId newEventId = GetNextEventId();

	m_mouseMoveEvents[newEventId] = event;
	return  newEventId;

}

void InputSystem::UnregisterEvent(EventId eventId)
{
	m_keyDownEvents.erase(eventId);
	m_keyUpEvents.erase(eventId);
	m_keyHoldEvents.erase(eventId);
	m_mouseMoveEvents.erase(eventId);
}
