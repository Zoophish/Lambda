#pragma once
#include <image/Texture.h>
#include <core/Spectrum.h>

struct FilmPixel {
	Spectrum spectrum = Spectrum(0);
	unsigned nSamples = 1;
};

typedef texture_t<FilmPixel> FilmData;

class Film {
	public:
		FilmData filmData;

		Film(const unsigned _width, const unsigned _height);

		/*
			Adds a spectral sample to pixel at coordinates _x and _y.
		*/
		void AddSample(const Spectrum &_s, const unsigned _x, const unsigned _y);

		/*
			Converts the accumulation of spetral samples on the film plane to
			the corresponding RGB counterparts divided by the sample count.
		*/
		void ToRGBTexture(Texture *_output) const;
};