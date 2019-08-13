#include <iostream>
//#include <core/TriangleMesh.h>
//#include <core/Scene.h>
//#include <core/Spectrum.h>
#include<cmath>
#include <image/Texture.h>
#include <sampling/HaltonSampler.h>

int main() {

	Texture tex(1024, 1024, Colour(0,0,0));
	SampleShifter sampleShifter;
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;
	for (unsigned i = 0; i < 800; ++i) {
		const Vec2 point = sampler.Get2D();
		tex.SetPixelCoord(point.x*1024, point.y*1024, Colour(1, 1, 1));
 		sampler.NextSample();
	}
	sampler.sampleShifter->pixelIndex++;
	for (unsigned i = 0; i < 800; ++i) {
		const Vec2 point = sampler.Get2D();
		tex.SetPixelCoord(point.x * 1024, point.y * 1024, Colour(1, 0, 0));
		sampler.NextSample();
	}
	
	tex.SaveToImageFile("out.png", false);

	system("pause");
	
	return 0;
}