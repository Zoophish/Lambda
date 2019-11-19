#pragma once
class MediaBoundary;
class Light;
class BxDF;

struct Material {
	//std::string tag / name;
	
	BxDF *bxdf;	//BxDF node?
	MediaBoundary *MediaBoundary;
	Light *light;
 };