#pragma once
#include <core/Spectrum.h>
#include <image/Texture.h>

class TextureAdapter {
	public:
	Texture *texture;
	SpectrumType type;

	TextureAdapter() {
		texture = nullptr;
		type = SpectrumType::Reflectance;
	}

	TextureAdapter(Texture *_texture, const SpectrumType _type = SpectrumType::Reflectance) {
		texture = _texture;
		type = _type;
	}

	Spectrum GetUV(const Vec2 &_uv) const {
		const Colour c = texture->GetPixelUV(_uv.x, _uv.y);
		const float tmp[3] = { c.r, c.g, c.b };
		return Spectrum::FromRGB(tmp, type);
	}
};