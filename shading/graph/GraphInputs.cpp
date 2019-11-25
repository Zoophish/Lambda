#include "GraphInputs.h"

namespace ShaderGraph {

	ScalarInput::ScalarInput(const Real _value) : Node(0, 1) {
		value = _value;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_SCALAR, this->GetScalar, "Scalar");
	}

	void ScalarInput::GetScalar(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Real *>(_out) = value;
	}



	RGBInput::RGBInput(const Colour &_rgb) : Node(0, 1) {
		rgb = _rgb;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, this->GetColour, "Colour");
	}

	RGBInput::RGBInput(const Real *_rgb) : Node(0,1) {
		rgb = _rgb;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, this->GetColour, "Colour");
	}

	void RGBInput::GetColour(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Colour *>(_out) = rgb;
	}



	ImageTextureInput::ImageTextureInput(Texture *_tex) : Node(0, 2) {
		tex = _tex;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, this->GetColour, "Colour");
		outputSockets[1] = MAKE_SOCKET(SocketType::TYPE_SCALAR, this->GetScalar, "Scalar");
	}

	void ImageTextureInput::GetColour(const SurfaceScatterEvent *_event, void *_out) const {
		const Vec2 uvs = maths::Fract(_event->hit->uvCoords);
		*reinterpret_cast<Colour *>(_out) = tex->GetPixelUV(uvs.x, uvs.y);
	}

	void ImageTextureInput::GetScalar(const SurfaceScatterEvent *_event, void *_out) const {
		const Vec2 uvs = maths::Fract(_event->hit->uvCoords);
		*reinterpret_cast<Real *>(_out) = tex->GetPixelUV(uvs.x, uvs.y).r;
	}


}