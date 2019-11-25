/* Virtual functions are avoided here with void*.
TODO: Benchmark virtual implementation vs void*.
*/

#pragma once
#include <stdexcept>
#include "../graph/GraphNode.h"
#include <core/Spectrum.h>
#include <image/Texture.h>

enum class TextureType {
	RGB,
	SPECTRAL,
	SCALAR,
	NONE
};

class TextureAdapter {
	public:
		SpectrumType type;

		TextureAdapter() {
			texture = nullptr;
			type = SpectrumType::Reflectance;
			textureType = TextureType::NONE;
		}

		inline void SetTexture(Texture *_texture) {
			texture = _texture;
			textureType = TextureType::RGB;
		}
		inline void SetTexture(texture_t<Spectrum> *_texture) {
			texture = _texture;
			textureType = TextureType::SPECTRAL;
		}
		inline void SetTexture(TextureR32 *_texture) {
			texture = _texture;
			textureType = TextureType::SCALAR;
		}

		Spectrum GetUV(const Vec2 &_uv) const {
			const Vec2 uv(maths::Fract(_uv.x), maths::Fract(_uv.y));
			switch (textureType) {
			case TextureType::RGB: {
				const Colour c = reinterpret_cast<Texture *>(texture)->GetPixelUV(uv.x, uv.y);
				const float tmp[3] = { c.r, c.g, c.b };
				return Spectrum::FromRGB(tmp, type);
			}
			case TextureType::SPECTRAL: {
				return reinterpret_cast<texture_t<Spectrum> *>(texture)->GetPixelUV(uv.x, uv.y);
			}
			case TextureType::SCALAR: {
				const Real scalar = reinterpret_cast<TextureR32 *>(texture)->GetPixelUV(uv.x, uv.y);
				return Spectrum(scalar);
			}
			default:
				throw std::runtime_error("A TextureAdapter with no texture had 'GetUV()' called.");
				return Spectrum(0);
			}
		}

		inline void operator=(Texture *_rhs) { SetTexture(_rhs); }
		inline void operator=(texture_t<Spectrum> *_rhs) { SetTexture(_rhs); }
		inline void operator=(TextureR32 *_rhs) { SetTexture(_rhs); }

	private:
		void *texture;
		TextureType textureType;
};