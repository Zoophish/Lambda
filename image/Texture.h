/*----	By Sam Warren 2019	----
		Generic texture class for synthesising images of colours. Implements stb_image library for
		loading / unloading of image files - http://nothings.org/stb. Supports HDR files.
	Texture types:
		- TextureRGBAFloat	-	32-bit float per channel, suitable for HDR.
*/

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "Colour.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party\stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION    
#include "third_party\stb_image.h"

#define GAMMA_POW 1 / 2.2

enum InterpolationMode {
	INTERP_NEAREST,
	INTERP_BILINEAR,
	INTERP_BICUBIC
};

template<class Format>
class texture_t {
	protected:
		unsigned width, height;
		unsigned interpolationMode = INTERP_BILINEAR;
		std::vector<Format> pixels;
		const TextureEncoding::TextureEncoder *encoder;
	
	public:
		texture_t(const unsigned _width = 1, const unsigned _height = 1) {
			width = _width;
			height = _height;
		}
	
		inline unsigned GetWidth() const { return width; }
		inline unsigned GetHeight() const { return height; }

		void Resize(const unsigned _width, const unsigned _height, const Format &_c = Format()) {
			width = _width;
			height = _height;
			pixels.resize(width * height, _c);
		}

		inline Format GetPixelCoord(const unsigned _x, const unsigned _y) const {
			return pixels[encoder->Map(_x, _y)];
		}

		inline void SetPixelCoord(const unsigned _x, const unsigned _y, const Colour &_c) {
			pixels[encoder->Map(_x, _y)] = _c;
		}

		Format GetPixelUV(const float _u, const float _v) const {
			switch (interpolationMode) {
			case INTERP_NEAREST :
				return GetPixelUVNearest(_u, _v);
			case INTERP_BILINEAR :
				return GetPixelUVBilinear(_u, _v);
			default:
				return GetPixelUVNearest(_u, _v);
			}
		}
		
		void SaveToImageFile(const char* name = "out.png", const bool gammaCorrect = true) const {
			const std::string sName = name;
			const unsigned dot = sName.find('.');
			const std::string format = sName.substr(dot+1);
			std::vector<uint32_t> p(width * height);
			for (unsigned int y = 0; y < height; ++y)
				for (unsigned int x = 0; x < width; ++x) {
					const Format pix = GetPixel(x, y);
					if (sizeof(Format) >= 32)
						float _r = pix.r;
					else
						_r = 0;
					if (sizeof(Format) >= 64)
						float _g = pix.g;
					else
						_g = 0;
					if (sizeof(Format) >= 96)
						float _b = pix.b;
					else
						_b = 0;
					if (sizeof(Format) >= 128)
						float _a = pix.a;
					else
						_a = 0;
					if(gammaCorrect) {
						_r = std::pow(_r, GAMMA_POW) * 256;
						_g = std::pow(_g, GAMMA_POW) * 256;
						_b = std::pow(_b, GAMMA_POW) * 256;
						_a *= 256;
					}
					else {
						_r *= 256;
						_g *= 256;
						_b *= 256;
						_a *= 256;
					}
					const int r = std::max(0, std::min(255, (int)_r));
					const int g = std::max(0, std::min(255, (int)_g));
					const int b = std::max(0, std::min(255, (int)_b));
					const int a = std::max(0, std::min(255, (int)_a));
					p[(y * width) + x] = (r << 24) + (g << 16) + (b << 8) + a;
				}
			if (format == "png") { stbi_write_png(name, width, height, 4, &p[0], width * 4); return; }
			if (format == "jpg") { stbi_write_jpg(name, width, height, 4, &p[0], 100); return; }
			if (format == "bmp") { stbi_write_bmp(name, width, height, 4, &p[0]); return; }
			if (format == "tga") { stbi_write_tga(name, width, height, 4, &p[0]); return; }
			std::cout << std::endl << "Did not save image.";
		}
	
		void LoadImageFile(const char* path, int channels = 4) {
			int file_width, file_height;
			if(stbi_info(path, &file_width, &file_height, nullptr)) {
				Resize(file_width, file_height);
				width = file_width; height = file_height;
				std::cout << std::endl << "Loading image...";
				int w = width, h = height;
				const float* data = stbi_loadf(path, &w, &h, &channels, 0);
				if(data != nullptr) {
					for (unsigned y = 0; y < height; ++y) {
						for (unsigned x = 0; x < width; ++x) {
							const int r = width * y * channels + x * channels;
							const int g = channels > 1 ? r + 1 : 0;
							const int b = channels > 2 ? r + 2 : 0;
							const int a = channels > 3 ? r + 3 : 0;
							const Format col(&data[r]);
							SetPixel(x, y, col);
						}
					}
					std::cout << std::endl << "Loaded: " << path;
				}
				else { std::cout << std::endl << "Could not load: " << path; }
			}
		}

		protected:
			inline Format GetPixelUVNearest(const float _u, const float _v) const {
				const unsigned int x = std::min((float)(width - 1), _u * width);
				const unsigned int y = std::min((float)(height - 1), _v * height);
				return GetPixelCoord(x, y);
			}

			inline Format GetPixelUVBilinear(const float _u, const float _v) const {
				const float fx = std::min((float)(width - 1), _u * width);
				const float fy = std::min((float)(height - 1), _v * height);
				const unsigned x = (unsigned)fx;
				const unsigned y = (unsigned)fy;
				const unsigned x1 = std::min(x + 1, width - 1);
				const unsigned y1 = std::min(y + 1, height - 1);
				const Format ya = Format::Lerp(GetPixelCoord(x, y), (x1, y), fx - (float)x);
				const Format yb = Format::Lerp(GetPixelCoord(x, y1), GetPixelCoord(x1, y1), fx - (float)x);
				return Format::Lerp(ya, yb, fy - (float)y);
			}
};

namespace TextureEncoding {

	class TextureEncoder {
		public:
			virtual size_t Map(const unsigned _x, const unsigned _y) const = 0;
	};

	class MortonEncoder : public TextureEncoder {
		public:
			inline uint64_t ShiftInterleave(const uint32_t _i) const {
				uint64_t word = _i;
				word = (word ^ (word << 16)) & 0x0000ffff0000ffff;
				word = (word ^ (word << 8)) & 0x00ff00ff00ff00ff;
				word = (word ^ (word << 4)) & 0x0f0f0f0f0f0f0f0f;
				word = (word ^ (word << 2)) & 0x3333333333333333;
				word = (word ^ (word << 1)) & 0x5555555555555555;
				return word;
			}

			size_t Map(const unsigned _x, const unsigned _y) const override {
				return ShiftInterleave(_x) | (ShiftInterleave(_y) << 1);
			}
	};

}

/*
Texture is a fast texture class for generic use. Uses Morton-order to improve cache read/write speeds.
*/
template<class Format>
class TextureMorton : public texture_t<Format> {
	public:
		TextureMorton(const unsigned _width = 1, const unsigned _height = 1, const Format &_c = Format()) : encoder(MortonEncoder()) {
			Resize(_width, _height, _c);
		}
};

typedef TextureMorton<Colour> Texture;
typedef TextureMorton<ColourFormat::RGBA32> TextureRGBA32;