/*----	By Sam Warren 2019	----
		Generic texture class for synthesising images of colours. Implements stb_image library for
		loading / unloading of image files - http://nothings.org/stb. Supports HDR files.
*/

#define GAMMA_POW 1 / 2.2

#pragma once
#include <iostream>
#include <vector>
#include <algorithm>
#include "Colour.h"
#include "TextureEncoding.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party\stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION    
#include "third_party\stb_image.h"

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
		size_t (*Map)(const unsigned, const unsigned, const unsigned, const unsigned) = TextureEncoding::ScanlineRowMap;
	
	public:
		InterpolationMode interpolationMode = InterpolationMode::INTERP_BILINEAR;

		texture_t<Format>(const unsigned _width = 1, const unsigned _height = 1, const Format &_c = Format()) {
			Resize(_width, _height, _c);
		}
	
		inline unsigned GetWidth() const { return width; }
		inline unsigned GetHeight() const { return height; }

		inline void Resize(const unsigned _width, const unsigned _height, const Format &_c = Format()) {
			width = _width;
			height = _height;
			pixels.resize(width * height, _c);
		}

		void SetEncoder(const EncoderMode _encoderMode) {
			switch (_encoderMode) {
			case EncoderMode::ENCODE_SCANLINEROW :
				Map = TextureEncoding::ScanlineRowMap; break;
			case EncoderMode::ENCODE_SCANLINECOL :
				Map = TextureEncoding::ScanlineColMap; break;
			case EncoderMode::ENCODE_MORTON :
				Map = TextureEncoding::MortonMap; break;
			case EncoderMode::ENCODE_HILBERT :
				Map = TextureEncoding::HilbertMap; break;
			default :
				Map = TextureEncoding::ScanlineRowMap;
			}
		}

		inline Format GetPixelCoord(const unsigned _x, const unsigned _y) const {
			return pixels[(*Map)(width, height, _x, _y)];
		}

		inline Format &Pixel(const unsigned _x, const unsigned _y) {
			return pixels[(*Map)(width, height, _x, _y)];
		}

		inline void SetPixelCoord(const unsigned _x, const unsigned _y, const Format &_c) {
			pixels[(*Map)(width, height, _x, _y)] = _c;
		}

		Format &operator[](const size_t _i) { return pixels[_i]; }
		Format operator[](const size_t _i) const { return pixels[_i]; }

		Format GetPixelUV(const float _u, const float _v) const {
			switch (interpolationMode) {
			case InterpolationMode::INTERP_NEAREST :
				return GetPixelUVNearest(_u, _v);
			case InterpolationMode::INTERP_BILINEAR :
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
					const Format pix = GetPixelCoord(x, y);
					float r, g, b, a;
						r = pix.r;
						g = pix.g;
						b = pix.b;
						a = 1;
					if(gammaCorrect) {
						r = std::pow(r, GAMMA_POW);
						g = std::pow(g, GAMMA_POW);
						b = std::pow(b, GAMMA_POW);
					}
					r *= 256;
					g *= 256;
					b *= 256;
					a *= 256;
					const int rI = std::max(0, std::min(255, (int)r));
					const int gI = std::max(0, std::min(255, (int)g));
					const int bI = std::max(0, std::min(255, (int)b));
					const int aI = std::max(0, std::min(255, (int)a));
					p[(y * width) + x] = (aI << 24) + (bI << 16) + (gI << 8) + rI;
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
							SetPixelCoord(x, y, Format(&data[r]));
						}
					}
					std::cout << std::endl << "Loaded: " << path;
				}
				else { std::cout << std::endl << "Could not load: " << path; }
			}
		}

		protected:
			inline Format GetPixelUVNearest(const float _u, const float _v) const {
				const unsigned int x = std::min((float)(width - 1), _u * (float)width);
				const unsigned int y = std::min((float)(height - 1), _v * (float)height);
				return GetPixelCoord(x, y);
			}

			inline Format GetPixelUVBilinear(const float _u, const float _v) const {
				const float fx = std::min((float)(width - 1), _u * (float)width);
				const float fy = std::min((float)(height - 1), _v * (float)height);
				const unsigned x = (unsigned)fx;
				const unsigned y = (unsigned)fy;
				const unsigned x1 = std::min(x + 1, width - 1);
				const unsigned y1 = std::min(y + 1, height - 1);
				const Format ya = Format::Lerp(GetPixelCoord(x, y), GetPixelCoord(x1, y), fx - (float)x);
				const Format yb = Format::Lerp(GetPixelCoord(x, y1), GetPixelCoord(x1, y1), fx - (float)x);
				return Format::Lerp(ya, yb, fy - (float)y);
			}
};

typedef texture_t<Colour> Texture;
typedef texture_t<ColourFormat::RGBA32> TextureRGBA32;