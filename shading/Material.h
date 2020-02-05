#pragma once
#include <utility/Memory.h>

LAMBDA_BEGIN

class MediaBoundary;
class Light;
class BxDF;

struct Material {	
	BxDF *bxdf;
	MediaBoundary *MediaBoundary;
	Light *light;
	MemoryArena graphArena;
 };

LAMBDA_END