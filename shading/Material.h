/*---- Sam Warren 2019-2020 ----
Encapsulation of visual properties of a material. Shader graph's for material should be kept here.

*/
#pragma once
#include <utility/Memory.h>
#include "media/Media.h"

LAMBDA_BEGIN

class Light;
class BxDF;

struct Material {
	std::string name;
	MediaBoundary MediaBoundary;
	BxDF *bxdf;
	Light *light;
	MemoryArena graphArena;
 };

LAMBDA_END