#pragma once
#include <DirectXMath.h>

#include "Entity.h"

DEFINE_ENTITY(Box, StaticMeshEntity,
	ENCLOSE(
public:
	void Init() override;
	)
);