#include "Material.h"

LAMBDA_BEGIN

Material::Material() {
	bxdf = nullptr;
	light = nullptr;
}

LAMBDA_END