#include "GraphInputs.h"

namespace ShaderGraph {

	/*
		---------- Scalar Input ----------
	*/

	ScalarInput::ScalarInput(const Real _value) : Node(0, 1, "Scalar Input") {
		value = _value;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &ScalarInput::GetScalar, "Scalar");
	}
	
	void ScalarInput::GetScalar(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Real *>(_out) = value;
	}

	/*
		---------- RGB Input ----------
	*/

	RGBInput::RGBInput(const Colour &_rgb) : Node(0, 1, "RGB Input") {
		rgb = _rgb;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, &RGBInput::GetColour, "Colour");
	}

	RGBInput::RGBInput(const Real *_rgb) : Node(0, 1, "RGB Input") {
		rgb = _rgb;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, &RGBInput::GetColour, "Colour");
	}

	void RGBInput::GetColour(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Colour *>(_out) = rgb;
	}

	/*
		---------- Spectral Input ----------
	*/

	SpectralInput::SpectralInput(Real *_lambda, Real *_vals, const unsigned _n) : Node(0, 1, "Spectrum") {
		for (unsigned i = 0; i < nSpectralSamples; ++i) {
			const Real l0 = maths::Lerp(sampledLambdaStart, sampledLambdaEnd, (Real)i * invNSpectralSamples);
			const Real l1 = maths::Lerp(sampledLambdaStart, sampledLambdaEnd, (Real)(i + 1) * invNSpectralSamples);
			spec[i] = AverageSpectrumSamples(_lambda, _vals, _n, l0, l1);
		}
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SPECTRUM, &SpectralInput::GetSpectrum, "Spectrum");
	}

	SpectralInput::SpectralInput(const Spectrum &_spec) : Node(0, 1) {
		spec = _spec;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SPECTRUM, &SpectralInput::GetSpectrum, "Spectrum");
	}

	void SpectralInput::GetSpectrum(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Spectrum *>(_out) = spec;
	}

	/*
		---------- Blackbody Input ----------
	*/

	BlackbodyInput::BlackbodyInput(Socket *_temperatureSocket, const unsigned _samples) : Node(1, 1, "Blackbody Spectrum") {
		samples = _samples;
		inputSockets[0] = MAKE_INPUT_SOCKET(SocketType::TYPE_SCALAR, _temperatureSocket);
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SPECTRUM, &BlackbodyInput::GetSpectrum, "Spectrum");
	}

	void BlackbodyInput::GetSpectrum(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Spectrum *>(_out) = MakeBlackbodySpectrum(inputSockets[0].socket->GetAsScalar(_event), samples);
	}

	void BlackbodyInput::Blackbody(const Real *_lambda, int _n, Real _T, Real *_Le) const {
		const Real c = 299792458;
		const Real h = 6.62606957e-34;
		const Real kb = 1.3806488e-23;
		for (unsigned i = 0; i < _n; ++i) {
			const Real l = _lambda[i] * 1e-9;
			const Real lambda5 = l * l * l * l * l;
			_Le[i] = ((Real)2 * h * c * c) / (lambda5 * (std::exp((h * c) / (l * kb * _T)) - (Real)1));
		} 
	}

	inline void BlackbodyInput::BlackbodyNormalized(const Real *_lambda, int _n, Real _temp, Real *_Le) const {
		Blackbody(_lambda, _n, _temp, _Le);
		const Real lambdaMax = 2.8977721e-3 / _temp * 1e9;
		Real maxL;
		Blackbody(&lambdaMax, 1, _temp, &maxL);
		for (unsigned i = 0; i < _n; ++i) _Le[i] /= maxL;
	} 

	inline Spectrum BlackbodyInput::MakeBlackbodySpectrum(const Real _temp, const unsigned _samples) const {
		std::unique_ptr<Real[]> lambdas(new Real[_samples]); //Shader memory arena.
		const int interval = sampledLambdaEnd - sampledLambdaStart;
		const Real invSamples = (Real)1 / _samples;
		for (unsigned i = 0; i < _samples; ++i) {
			lambdas[i] = (Real)sampledLambdaStart + (Real)interval * invSamples * (Real)i;
		}
		std::unique_ptr<Real[]> v(new Real[_samples]);
		BlackbodyNormalized(&lambdas[0], _samples, _temp, &v[0]);
		return Spectrum::FromSampled(&lambdas[0], &v[0], _samples);
	}

	/*
		---------- Image-Texture Input ----------
	*/

	ImageTextureInput::ImageTextureInput(Texture *_tex) : Node(0, 2, "Image Texture") {
		tex = _tex;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, &ImageTextureInput::GetColour, "Colour");
		outputSockets[1] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &ImageTextureInput::GetScalar, "Scalar");
	}

	void ImageTextureInput::GetColour(const SurfaceScatterEvent *_event, void *_out) const {
		const Vec2 uvs = maths::Fract(_event->hit->uvCoords);
		*reinterpret_cast<Colour *>(_out) = tex->GetPixelUV(uvs.x, uvs.y);
	}

	void ImageTextureInput::GetScalar(const SurfaceScatterEvent *_event, void *_out) const {
		const Vec2 uvs = maths::Fract(_event->hit->uvCoords);
		*reinterpret_cast<Real *>(_out) = tex->GetPixelUV(uvs.x, uvs.y).r;
	}

	/*
		--------- Image-Texture-Channel Input ---------
	*/

	ImageTextureChannelInput::ImageTextureChannelInput(Texture *_tex, const uint8_t _channel) : Node(0, 1, "Image Channel") {
		tex = _tex;
		channel = _channel;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &ImageTextureChannelInput::GetScalar, "Scalar");
	}

	void ImageTextureChannelInput::GetScalar(const SurfaceScatterEvent *_event, void *_out) const {
		const Vec2 uvs = maths::Fract(_event->hit->uvCoords);
		*reinterpret_cast<Real *>(_out) = tex->GetPixelUV(uvs.x, uvs.y)[channel];
	}

	/*
		--------- Spectral-Texture Input ----------
	*/

	SpectralTextureInput::SpectralTextureInput(texture_t<Spectrum> *_tex) : Node(0, 1, "Spectral Texture") {
		tex = _tex;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SPECTRUM, &SpectralTextureInput::GetSpectrum, "Spectrum");
	}

	void SpectralTextureInput::GetSpectrum(const SurfaceScatterEvent *_event, void *_out) const {
		const Vec2 uvs = maths::Fract(_event->hit->uvCoords);
		*reinterpret_cast<Spectrum *>(_out) = tex->GetPixelUV(uvs.x, uvs.y);
	}
}