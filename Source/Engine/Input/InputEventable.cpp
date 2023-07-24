#include "InputEventable.h"

#define IS InputSystem::GetInstance()

void InputEventable::RegisterOnKeyDown(const OnKeyEvent& event)
{
	if(OnKeyDownId != 0)
	{
		IS.UnregisterEvent(OnKeyDownId);
	}
	OnKeyDownId = IS.RegisterOnKeyDown(event);
}

void InputEventable::RegisterOnKeyUp(const OnKeyEvent& event)
{
	if (OnKeyUpId != 0)
	{
		IS.UnregisterEvent(OnKeyUpId);
	}
	OnKeyUpId = IS.RegisterOnKeyDown(event);
}

void InputEventable::RegisterOnKeyHold(const OnKeyEvent& event)
{
	if (OnKeyHoldId != 0)
	{
		IS.UnregisterEvent(OnKeyHoldId);
	}
	OnKeyHoldId = IS.RegisterOnKeyHold(event);
}

void InputEventable::RegisterOnMouseMove(const OnMouseMoveEvent& event)
{
	if (OnMouseMoveId != 0)
	{
		IS.UnregisterEvent(OnMouseMoveId);
	}
	OnMouseMoveId = IS.RegisterOnMouseMove(event);
}

void InputEventable::UnregisterAll() const
{
	IS.UnregisterEvent(OnKeyDownId);
	IS.UnregisterEvent(OnKeyUpId);
	IS.UnregisterEvent(OnKeyHoldId);
	IS.UnregisterEvent(OnMouseMoveId);
}

InputEventable::~InputEventable()
{
	UnregisterAll();
}
