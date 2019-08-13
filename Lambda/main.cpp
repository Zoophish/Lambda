#include <iostream>
//#include <core/TriangleMesh.h>
//#include <core/Scene.h>
//#include <core/Spectrum.h>
#include<cmath>
#include <image/Texture.h>
#include <sampling/HaltonSampler.h>

int main() {

	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("C:/Users/user/source/repos/Lambda/content/HDR_RGBA_63.png");

	Texture tex(512, 512, Colour(0,0,0));

	SampleShifter sampleShifter(&blueNoise);
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	const Colour c[8] = { Colour(1,1,1), Colour(0,1,1), Colour(1,0,1), Colour(1,1,0), Colour(1,0,0), Colour(0,1,0), Colour(0,0,1), Colour(.5,1,.2) };

	for (unsigned j = 0; j < 8; ++j) {
		for (unsigned i = 0; i < 800; ++i) {
			const Vec2 point = sampler.Get2D();
			tex.SetPixelCoord(point.x * 512, point.y * 512, c[j]);
			sampler.NextSample();
		}
		sampler.SetSample(0);
		sampleShifter.pixelIndex++;
	}
	
	tex.SaveToImageFile("out.png", false);

	system("pause");
	
	return 0;
}