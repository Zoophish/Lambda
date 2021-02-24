#include <iostream>
#include "Film.h"

LAMBDA_BEGIN

Film::Film(const unsigned _width, const unsigned _height) {
	filmData.Resize(_width, _height);
	Clear();
}

void Film::ToRGBTexture(Texture *_output) const {
	if (filmData.GetWidth() == _output->GetWidth() && filmData.GetHeight() == _output->GetHeight()) {
		const size_t size = filmData.GetWidth() * filmData.GetHeight();
		for (size_t i = 0; i < size; ++i) {
			float xyz[3];
			const Spectrum out = (Spectrum)(filmData[i].spectrum / (Real)filmData[i].nSamples);
			out.ToRGB(xyz);
			Colour c(&xyz[0]);
			c.a = 1;
			(*_output)[i] = c;
		}
	}
	else std::cout << std::endl << "Output texture size does not match film size.";
}

void Film::Clear() {
	for (unsigned i = 0; i < filmData.GetWidth() * filmData.GetHeight(); ++i) {
		filmData[i] = { Spectrum(0), 0 };
	}
}

LAMBDA_END