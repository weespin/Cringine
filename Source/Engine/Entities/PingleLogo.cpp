#include "PingleLogo.h"

void PingleLogo::Update(float nDeltaTime)
{
	__super::Update(nDeltaTime);
	GetTransform().Rotate(0.f, 0.f, 10.f);
}
