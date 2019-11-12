#pragma once
class MediaBoundary;
class Light;
class BxDF;


struct Material {
	BxDF *bxdf;
	MediaBoundary *MediaBoundary;
	Light *light;
 };