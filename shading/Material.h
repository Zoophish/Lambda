#pragma once
#include <utility/Memory.h>
#include "media/Media.h"

LAMBDA_BEGIN

class Light;
class BxDF;

struct Material {	
	MediaBoundary MediaBoundary;
	BxDF *bxdf;
	Light *light;
	MemoryArena graphArena;
 };

LAMBDA_END