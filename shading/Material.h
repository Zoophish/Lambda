#pragma once
#include <utility/Memory.h>
class MediaBoundary;
class Light;
class BxDF;

struct Material {	
	BxDF *bxdf;
	MediaBoundary *MediaBoundary;
	Light *light;
	MemoryArena graphArena;
 };