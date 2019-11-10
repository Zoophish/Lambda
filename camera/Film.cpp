#include <iostream>
#include "Film.h"

Film::Film(const unsigned _width, const unsigned _height) {
	filmData.Resize(_width, _height);
}

void Film::AddSample(const Spectrum &_s, const unsigned _x, const unsigned _y) {
	filmData.Pixel(_x, _y).spec += _s;
	filmData.Pixel(_x, _y).sampleCount++;
}

void Film::ToRGBTexture(Texture *_output) const {
	if (filmData.GetWidth() == _output->GetWidth() && filmData.GetHeight() == _output->GetHeight()) {
		const size_t size = filmData.GetWidth() * filmData.GetHeight();
		for (size_t i = 0; i < size; ++i) {
			float xyz[3];
			const Spectrum out = (Spectrum)(filmData[i].spec / (Real)filmData[i].sampleCount);
			out.ToRGB(xyz);

			(*_output)[i] = Colour(&xyz[0]);
		}
	}
	else std::cout << std::endl << "Output texture size does not match film size.";
}