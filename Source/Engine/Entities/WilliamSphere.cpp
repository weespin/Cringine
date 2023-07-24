#include "WilliamSphere.h"


void WilliamSphere::Update(float nDeltaTime)
{
	__super::Update(nDeltaTime);

	GetTransform().Rotate(0.f, 2.f, 0.f);
}
