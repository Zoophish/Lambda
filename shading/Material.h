#pragma once
class MediaBoundary;
class Light;
class BxDF;

struct Material {
	//std::string tag / name;
	BxDF *bxdf;
	MediaBoundary *MediaBoundary;
	Light *light;
 };