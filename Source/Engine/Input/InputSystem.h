#pragma once

#include <vector>
#include <functional>

#include "Application.h"

using EventId = unsigned long long;
using OnKeyEvent = std::function<void(char)>;
using OnMouseMoveEvent = std::function<void(int, int)>;

class InputSystem
{
public:
    static InputSystem& GetInstance() {
        static InputSystem instance;
        return instance;
    }

    void ProcessInput(HWND hWnd, UINT msg, WPARAM wParam, LPARAM lParam);

    //SHOULD BE CALLED ONLY FROM GAME UPDATE LOGIC!
    void UpdateHold();

private:
    EventId RegisterOnKeyDown(const OnKeyEvent& event);
    EventId RegisterOnKeyUp(const OnKeyEvent& event);
    EventId RegisterOnKeyHold(const OnKeyEvent& event);
    EventId RegisterOnMouseMove(const OnMouseMoveEvent& event);
    std::vector<char> m_holdedKeys{};
    std::unordered_map<EventId, OnKeyEvent> m_keyDownEvents{};
	std::unordered_map<EventId, OnKeyEvent> m_keyUpEvents{};
    std::unordered_map<EventId, OnKeyEvent> m_keyHoldEvents{};
    std::unordered_map<EventId, OnMouseMoveEvent> m_mouseMoveEvents{};
    void UnregisterEvent(EventId eventId);
    InputSystem() = default;
    InputSystem(const InputSystem&) = delete;
    InputSystem& operator=(const InputSystem&) = delete;
    EventId GetNextEventId() { return ++m_CurrentEventId; }
    EventId m_CurrentEventId = 0;
    friend class InputEventable;
};

