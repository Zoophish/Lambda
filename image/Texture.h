/*----	By Sam Warren 2019	----
		Generic template texture class for arbitrary texel types.
			- Includes specialised RGB32 colour texture type which implements stb_image library for
			loading / saving of image files - http://nothings.org/stb.
*/

#define GAMMA_POW 1 / 2.2

#pragma once
#include <vector>
#include <algorithm>
#include <maths/maths.h>
#include "Colour.h"
#include "TextureEncoding.h"

enum class InterpolationMode {
	INTERP_NEAREST,
	INTERP_BILINEAR,
	INTERP_BICUBIC
};

enum class EncoderMode {
	ENCODE_SCANLINEROW,
	ENCODE_SCANLINECOL,
	ENCODE_MORTON,
	ENCODE_HILBERT,
};

template<class Format>
class texture_t {
	protected:
		unsigned width, height;
		std::vector<Format> pixels;
		size_t(*Order)(const unsigned, const unsigned, const unsigned, const unsigned) = TextureEncoding::ScanlineRowOrder;

		inline Format GetPixelUVNearest(const float _u, const float _v) const {
			const unsigned int x = std::min((float)(width - 1), maths::Clamp(_u, 0.f, 1.f) * (float)width);
			const unsigned int y = std::min((float)(height - 1), maths::Clamp(_v, 0.f, 1.f) * (float)height);
			return GetPixelCoord(x, y);
		}

		inline Format GetPixelUVBilinear(const float _u, const float _v) const {
			const float fx = std::min((float)(width - 1), maths::Clamp(_u, 0.f, 1.f) * (float)width);
			const float fy = std::min((float)(height - 1), maths::Clamp(_v, 0.f, 1.f) * (float)height);
			const unsigned x = (unsigned)fx;
			const unsigned y = (unsigned)fy;
			const unsigned x1 = std::min(x + 1, width - 1);
			const unsigned y1 = std::min(y + 1, height - 1);
			const Format ya = maths::Lerp(GetPixelCoord(x, y), GetPixelCoord(x1, y), fx - (float)x);
			const Format yb = maths::Lerp(GetPixelCoord(x, y1), GetPixelCoord(x1, y1), fx - (float)x);
			return maths::Lerp(ya, yb, fy - (float)y);
		}

	public:
		InterpolationMode interpolationMode = InterpolationMode::INTERP_BILINEAR;

		texture_t<Format>(const unsigned _width = 1, const unsigned _height = 1, const Format & _c = Format()) {
			Resize(_width, _height, _c);
		}
	
		inline unsigned GetWidth() const { return width; }

		inline unsigned GetHeight() const { return height; }

		/*
			Resizes texture and clears all texels to _c.
		*/
		inline void Resize(const unsigned _width, const unsigned _height, const Format &_c = Format()) {
			width = _width;
			height = _height;
			pixels.resize(width * height, _c);
		}

		/*
			Sets the method in which texels are ordered.
				- Will not work if encoder changes after texture is made with a different encoder.
		*/
		inline void SetEncoder(const EncoderMode _encoderMode) {
			switch (_encoderMode) {
			case EncoderMode::ENCODE_SCANLINEROW:
				Order = TextureEncoding::ScanlineRowOrder; break;
			case EncoderMode::ENCODE_SCANLINECOL:
				Order = TextureEncoding::ScanlineColOrder; break;
			case EncoderMode::ENCODE_MORTON:
				Order = TextureEncoding::MortonOrder; break;
			case EncoderMode::ENCODE_HILBERT:
				Order = TextureEncoding::HilbertOrder; break;
			default:
				Order = TextureEncoding::ScanlineRowOrder;
			}
		}

		inline Format GetPixelCoord(const unsigned _x, const unsigned _y) const {
			return pixels[(*Order)(width, height, _x, _y)];
		}

		inline Format &Pixel(const unsigned _x, const unsigned _y) {
			return pixels[(*Order)(width, height, _x, _y)];
		}

		inline void SetPixelCoord(const unsigned _x, const unsigned _y, const Format &_c) {
			pixels[(*Order)(width, height, _x, _y)] = _c;
		}

		inline Format &operator[](const size_t _i) { return pixels[_i]; }
		inline Format operator[](const size_t _i) const { return pixels[_i]; }
		
		inline Format GetPixelUV(const float _u, const float _v) const {
			switch (interpolationMode) {
			case InterpolationMode::INTERP_NEAREST :
				return GetPixelUVNearest(_u, _v);
			case InterpolationMode::INTERP_BILINEAR :
				return GetPixelUVBilinear(_u, _v);
			default:
				return GetPixelUVNearest(_u, _v);
			}
		}
};


/*
	STB Image compatable texture type.
*/
template<>
class texture_t<Colour> {
	protected:
		unsigned width, height;
		std::vector<Colour> pixels;
		size_t(*Order)(const unsigned, const unsigned, const unsigned, const unsigned) = TextureEncoding::ScanlineRowOrder;

		inline Colour GetPixelUVNearest(const float _u, const float _v) const {
			const unsigned int x = std::min((float)(width - 1), maths::Clamp(_u, 0.f, 1.f) * (float)width);
			const unsigned int y = std::min((float)(height - 1), maths::Clamp(_v, 0.f, 1.f) * (float)height);
			return GetPixelCoord(x, y);
		}

		inline Colour GetPixelUVBilinear(const float _u, const float _v) const {
			const float fx = std::min((float)(width - 1), maths::Clamp(_u, 0.f, 1.f) * (float)width);
			const float fy = std::min((float)(height - 1), maths::Clamp(_v, 0.f, 1.f) * (float)height);
			const unsigned x = (unsigned)fx;
			const unsigned y = (unsigned)fy;
			const unsigned x1 = std::min(x + 1, width - 1);
			const unsigned y1 = std::min(y + 1, height - 1);
			const Colour ya = maths::Lerp(GetPixelCoord(x, y), GetPixelCoord(x1, y), fx - (float)x);
			const Colour yb = maths::Lerp(GetPixelCoord(x, y1), GetPixelCoord(x1, y1), fx - (float)x);
			return maths::Lerp(ya, yb, fy - (float)y);
		}
	
	public:
		InterpolationMode interpolationMode = InterpolationMode::INTERP_BILINEAR;

		texture_t(const unsigned _width = 1, const unsigned _height = 1, const Colour & _c = Colour()) {
			Resize(_width, _height, _c);
		}

		inline unsigned GetWidth() const { return width; }

		inline unsigned GetHeight() const { return height; }

		/*
			Resizes texture and clears all texels to _c.
		*/
		inline void Resize(const unsigned _width, const unsigned _height, const Colour &_c = Colour()) {
			width = _width;
			height = _height;
			pixels.resize(width * height, _c);
		}

		/*
			Sets the method in which texels are ordered.
				- Will not work if encoder changes after texture is made with a different encoder.
		*/
		inline void SetEncoder(const EncoderMode _encoderMode) {
			switch (_encoderMode) {
			case EncoderMode::ENCODE_SCANLINEROW:
				Order = TextureEncoding::ScanlineRowOrder; break;
			case EncoderMode::ENCODE_SCANLINECOL:
				Order = TextureEncoding::ScanlineColOrder; break;
			case EncoderMode::ENCODE_MORTON:
				Order = TextureEncoding::MortonOrder; break;
			case EncoderMode::ENCODE_HILBERT:
				Order = TextureEncoding::HilbertOrder; break;
			default:
				Order = TextureEncoding::ScanlineRowOrder;
			}
		}

		inline Colour GetPixelCoord(const unsigned _x, const unsigned _y) const {
			return pixels[(*Order)(width, height, _x, _y)];
		}

		inline Colour &Pixel(const unsigned _x, const unsigned _y) {
			return pixels[(*Order)(width, height, _x, _y)];
		}

		inline void SetPixelCoord(const unsigned _x, const unsigned _y, const Colour &_c) {
			pixels[(*Order)(width, height, _x, _y)] = _c;
		}

		inline Colour &operator[](const size_t _i) { return pixels[_i]; }
		inline Colour operator[](const size_t _i) const { return pixels[_i]; }

		inline Colour GetPixelUV(const float _u, const float _v) const {
			switch (interpolationMode) {
			case InterpolationMode::INTERP_NEAREST:
				return GetPixelUVNearest(_u, _v);
			case InterpolationMode::INTERP_BILINEAR:
				return GetPixelUVBilinear(_u, _v);
			default:
				return GetPixelUVNearest(_u, _v);
			}
		}

		void SaveToImageFile(const char *_path, const bool _gammaCorrect = true, const bool _alpha = true) const;

		void LoadImageFile(const char *_path, int _channels = 4);
};

/*
	Generic texture template typdefs
*/

typedef texture_t<Colour> Texture;
typedef texture_t<float> TextureR32;