#pragma once
#include "ShaderGraph.h"
#include <image/Texture.h>
#include <shading/ScatterEvent.h>

LAMBDA_BEGIN

namespace ShaderGraph {

	class ScalarInput : public Node {
		public:
			Real value;

			ScalarInput(const Real _value);

			void GetScalar(const ScatterEvent *_event, void *_out) const;
	};



	class Vec2Input : public Node {
		public:
			Vec2 vec2;

			Vec2Input(const Vec2 &_vec);

			void GetVec2(const ScatterEvent *_event, void *_out) const;
	};



	class RGBInput : public Node {
		public:
			Colour rgb;

			RGBInput(const Colour &_rgb);
			RGBInput(const Real *_rgb);

			void GetColour(const ScatterEvent *_event, void *_out) const;
	};



	class SpectralInput : public Node {
		public:
			Spectrum spec;

			SpectralInput(Real *_lambda, Real *_vals, const unsigned _n);

			SpectralInput(const Spectrum &_spec);

			void GetSpectrum(const ScatterEvent *_event, void *_out) const;
	};



	class BlackbodyInput : public Node {
		public:
			unsigned samples;

			BlackbodyInput(Socket* _temperatureSocket, const unsigned _samples = Spectrum::nSamples);

			void GetSpectrum(const ScatterEvent *_event, void *_out) const;

		private:
			void Blackbody(const Real *_lambda, int _n, Real _T, Real *_Le) const;

			inline void BlackbodyNormalized(const Real *_lambda, int _n, Real _T, Real *_Le) const;

			inline Spectrum MakeBlackbodySpectrum(const Real _temp, const unsigned _samples) const;
	};



	class ImageTextureInput : public Node {
		public:
			Texture *tex;

			ImageTextureInput(Texture *_tex);

			void GetColour(const ScatterEvent *_event, void *_out) const;

			void GetScalar(const ScatterEvent *_event, void *_out) const;
	};



	class ImageTextureChannelInput : public Node {
		public:
			Texture *tex;
			uint8_t channel;

			ImageTextureChannelInput(Texture *_tex, const uint8_t _channel = 0);

			void GetScalar(const ScatterEvent *_event, void *_out) const;
	};


	class SpectralTextureInput : public Node {
		public:
			texture_t<Spectrum> *tex;

			SpectralTextureInput(texture_t<Spectrum> *_tex);

			void GetSpectrum(const ScatterEvent *_event, void *_out) const;
	};
}

LAMBDA_END