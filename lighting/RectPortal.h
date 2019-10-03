#pragma once
#include "EnvironmentLight.h"

class RectPortal : public Light {
	public:
		EnvironmentLight *parentLight;
};