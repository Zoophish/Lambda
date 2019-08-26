#pragma once
#include <image/Texture.h>
#include <core/Spectrum.h>

struct FilmPixel {
	Spectrum spec = Spectrum(0);
	unsigned sampleCount = 1;
};

typedef texture_t<FilmPixel> FilmData;

class Film {
	public:
		FilmData filmData;

		 Film(const unsigned _width, const unsigned _height) {
			 filmData.Resize(_width, _height);
		 }

		 void AddSample(const Spectrum &_s, const unsigned _x, const unsigned _y) {
			 filmData.Pixel(_x, _y).spec += _s;
			 filmData.Pixel(_x, _y).sampleCount++;
		 }

		 void ToXYZTexture(Texture *_output) const {
			 if (filmData.GetWidth() == _output->GetWidth() && filmData.GetHeight() == _output->GetHeight()) {
				 const size_t size = filmData.GetWidth() * filmData.GetHeight();
				 for (size_t i = 0; i < size; ++i) {
					 float xyz[3];
					 const Spectrum out = (Spectrum)(filmData[i].spec / (Real)filmData[i].sampleCount);
					 out.ToRGB(xyz);
					 //std::cout << xyz[0] << std::endl << xyz[1] << std::endl << xyz[2];
					 (*_output)[i] = Colour(xyz);
				 }
			 }
			 else
				 std::cout << std::endl << "Output texture size does not match film size.";
		 }
};