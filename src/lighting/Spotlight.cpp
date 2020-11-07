#include "Spotlight.h"

LAMBDA_BEGIN

Spotlight::Spotlight() {
	axis = Vec3(0, -1, 0);
	cosConeAngle = std::cos(PI * .5);
	cosFalloffStart = std::cos(PI * .4);
	intensity = 1;
	emission = nullptr;
}

Spotlight::Spotlight(const Vec3 &_axis, const Real _coneAngle, const Real _falloffStart) {

}

LAMBDA_END