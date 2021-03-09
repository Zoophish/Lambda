// Sam Warren 2021
#pragma once
#include <algorithm>
#include <memory>
#include <vector>
#include <maths/maths.h>
#include "Colour.h"
#include "TextureEncoding.h"

/* Use BMI instructions for Hilbert encoding (faster). */
#define USE_BMI_INSTRUCTIONS

#ifdef USE_BMI_INSTRUCTIONS
#include <immintrin.h>
#endif


LAMBDA_BEGIN

constexpr float GAMMA_POW = 1 / 2.2;

enum class InterpolationMode {
	INTERP_NEAREST,
	INTERP_BILINEAR,
	INTERP_BICUBIC
};

enum class EncodingMode {
	ENCODE_SCANLINEROW,
	ENCODE_SCANLINECOL,
	ENCODE_MORTON,
	ENCODE_HILBERT,
};

template<class Type>
class texture_t {
	protected:
		unsigned width, height;
		std::unique_ptr<Type[]> data;

		/* width, height = 2^n */
		static inline size_t MortonOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
			#ifdef USE_BMI_INSTRUCTIONS
				static const uint64_t x2Mask = 0xAAAAAAAAAAAAAAAA;
				static const uint64_t y2Mask = 0x5555555555555555;
				return _pdep_u64(_y, y2Mask) | _pdep_u64(_x, x2Mask);
			#else
				return TextureEncoding::ShiftInterleave(_x) | (TextureEncoding::ShiftInterleave(_y) << 1);
			#endif
		}

		/* width = height = 2^n */
		static inline size_t HilbertOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
			return TextureEncoding::HilbertXYToIndex(_w, _x, _y);
		}

		static inline size_t ScanlineRowOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
			return _y * _w + _x;
		}

		static inline size_t ScanlineColOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y) {
			return _x * _h + _y;
		}

		inline Type GetPixelUVNearest(const float _u, const float _v) const {
			const unsigned int x = std::min((float)(width - 1), maths::Clamp(_u, 0.f, 1.f) * (float)width);
			const unsigned int y = std::min((float)(height - 1), maths::Clamp(_v, 0.f, 1.f) * (float)height);
			return GetPixelCoord(x, y);
		}

		inline Type GetPixelUVBilinear(const float _u, const float _v) const {
			const float fx = std::min((float)(width - 1), maths::Clamp(_u, 0.f, 1.f) * (float)width);
			const float fy = std::min((float)(height - 1), maths::Clamp(_v, 0.f, 1.f) * (float)height);
			const unsigned x = (unsigned)fx;
			const unsigned y = (unsigned)fy;
			const unsigned x1 = std::min(x + 1, width - 1);
			const unsigned y1 = std::min(y + 1, height - 1);
			const Type ya = maths::Lerp(GetPixelCoord(x, y), GetPixelCoord(x1, y), fx - (float)x);
			const Type yb = maths::Lerp(GetPixelCoord(x, y1), GetPixelCoord(x1, y1), fx - (float)x);
			return maths::Lerp(ya, yb, fy - (float)y);
		}

	public:
		InterpolationMode interpolation = InterpolationMode::INTERP_BILINEAR;
		EncodingMode encoding = EncodingMode::ENCODE_SCANLINEROW;

		texture_t<Type>(const unsigned _width = 1, const unsigned _height = 1, const Type & _c = Type()) {
			Resize(_width, _height, _c);
		}

		template<class Type2>
		texture_t<Type>(const texture_t<Type2> &_texture) {
			if (width > _texture.width && height > _texture.height) {
				for (unsigned y = 0; y < width; ++y) {
					for (unsigned x = 0; x < width; ++x) {
						SetPixelCoord(x, y, Type(&_texture.GetPixelCoord(x, y)));
					}
				}
			}
		}

		static texture_t<Type> Copy(const texture_t<Type> &_texture) {
			texture_t<Type> copy;
			copy.width = _texture.width;
			copy.height = _texture.height;
			copy.encoding = _texture.encoding;
			copy.data.reset(new Type[_texture.width * _texture.height]);
			memcpy(&copy.data[0], &_texture.data[0], sizeof(Type) * _texture.width * _texture.height);
			return copy;
		}

		inline unsigned GetWidth() const { return width; }

		inline unsigned GetHeight() const { return height; }

		/*
			Resizes texture and clears all texels to _c.
		*/
		inline void Resize(const unsigned _width, const unsigned _height, const Type &_c = Type()) {
			width = _width;
			height = _height;
			data.reset(new Type[width * height]);
			std::fill_n(&data[0], width * height, _c);
		}

		inline Type GetPixelCoord(const unsigned _x, const unsigned _y) const {
			switch (encoding) {
			case EncodingMode::ENCODE_SCANLINEROW:
				return data[ScanlineRowOrder(width, height, _x, _y)];
			case EncodingMode::ENCODE_SCANLINECOL:
				return data[ScanlineColOrder(width, height, _x, _y)];
			case EncodingMode::ENCODE_HILBERT:
				return data[HilbertOrder(width, height, _x, _y)];
			case EncodingMode::ENCODE_MORTON:
				return data[MortonOrder(width, height, _x, _y)];
			}
		}

		inline Type &GetPixelCoord(const unsigned _x, const unsigned _y) {
			switch (encoding) {
			case EncodingMode::ENCODE_SCANLINEROW:
				return data[ScanlineRowOrder(width, height, _x, _y)];
			case EncodingMode::ENCODE_SCANLINECOL:
				return data[ScanlineColOrder(width, height, _x, _y)];
			case EncodingMode::ENCODE_HILBERT:
				return data[HilbertOrder(width, height, _x, _y)];
			case EncodingMode::ENCODE_MORTON:
				return data[MortonOrder(width, height, _x, _y)];
			}
		}

		inline void SetPixelCoord(const unsigned _x, const unsigned _y, const Type &_c) {
			GetPixelCoord(_x, _y) = _c;
		}

		inline Type &operator[](const size_t _i) { return data[_i]; }
		inline Type operator[](const size_t _i) const { return data[_i]; }
		
		inline Type GetPixelUV(const float _u, const float _v) const {
			switch (interpolation) {
			case InterpolationMode::INTERP_NEAREST :
				return GetPixelUVNearest(_u, _v);
			case InterpolationMode::INTERP_BILINEAR :
				return GetPixelUVBilinear(_u, _v);
			default:
				return GetPixelUVNearest(_u, _v);
			}
		}

		inline void *GetData() {
			return (void *)&data[0];
		}
};


class Texture : public texture_t<Colour> {
	private:
		void ParseData(float *_data, const int _channels);

	public:
		Texture(const unsigned _w = 1, const unsigned _h = 1, const Colour &_c = Colour());

		static Texture Copy(const Texture &_texture);

		static bool GetFileInfo(const char *_path, int *_width, int *_height, int *_channels);

		static bool GetMemoryInfo(const void *_src, const int _size, int *_w, int *_h, int *_channels);

		void SaveToImageFile(const char *_path, const bool _gammaCorrect = true, const bool _alpha = true) const;

		void LoadImageFile(const char *_path, int _channels = 4);

		void LoadFromMemory(const void *_src, const int _size, const int _channels = 4);
};


/*
	Generic texture template typdefs
*/

typedef texture_t<float> TextureR32;

LAMBDA_END