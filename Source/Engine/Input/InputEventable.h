#pragma once
#include "InputSystem.h"

class InputEventable
{
public:
	void RegisterOnKeyDown(const OnKeyEvent& event);

	void RegisterOnKeyUp(const OnKeyEvent& event);

	void RegisterOnKeyHold(const OnKeyEvent& event);

	void RegisterOnMouseMove(const OnMouseMoveEvent& event);

	void UnregisterAll() const;

	~InputEventable();

private:
	EventId OnKeyDownId = 0;
	EventId OnKeyUpId = 0;
	EventId OnKeyHoldId = 0;
	EventId OnMouseMoveId = 0;
};
