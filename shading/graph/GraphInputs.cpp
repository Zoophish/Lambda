#include "GraphInputs.h"

namespace ShaderGraph {

	RGBInput::RGBInput(const Colour &_rgb) : Node(0, 1) {
		rgb = _rgb;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, &this->GetColour, "Colour");
	}

	void RGBInput::GetColour(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Colour *>(_out) = rgb;
	}



	TextureInput::TextureInput(Texture *_tex) : Node(0, 2) {
		tex = _tex;
		outputSockets[0] = MAKE_SOCKET(SocketType::TYPE_COLOUR, &this->GetColour, "Colour");
		outputSockets[1] = MAKE_SOCKET(SocketType::TYPE_SCALAR, &this->GetScalar, "Scalar");
	}

	void TextureInput::GetColour(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Colour *>(_out) = tex->GetPixelUV(_event->hit->uvCoords.x, _event->hit->uvCoords.y);
	}

	void TextureInput::GetScalar(const SurfaceScatterEvent *_event, void *_out) const {
		*reinterpret_cast<Real *>(_out) = tex->GetPixelUV(_event->hit->uvCoords.x, _event->hit->uvCoords.y).r;
	}


}