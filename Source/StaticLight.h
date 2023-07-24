#pragma once
#include <DirectXMath.h>

#include "EngineHeaders.h"
#include "PropertyManager.h"

class StaticLight : public IPropertyManager 
{
public:
	StaticLight()
	{
		
	}
	CREATE_PROPERTY(DirectX::XMFLOAT3, dir, ENCLOSE({ 0.5,0.5,0.5 }));
	CREATE_PROPERTY(ColorHelper::RGBA, ambient, ENCLOSE({ 0.4,0.4,0.4 }));
	CREATE_PROPERTY(ColorHelper::RGBA, diffuse, ENCLOSE({0.3,0.3,0.3}));
};
