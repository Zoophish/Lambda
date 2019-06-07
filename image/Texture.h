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

class Texture {
	protected:
		unsigned width, height;
	
	public:
		Texture() {}
	
		inline unsigned GetWidth() const { return width; }
		inline unsigned GetHeight() const { return height; }
	

		virtual Colour GetPixelByIndex(const size_t _i) const = 0;
	
		virtual void SetPixelByIndex(const size_t i, const Colour &c) = 0;

		virtual void Resize(const unsigned _x, const unsigned _y, const Colour &_c = Colour()) = 0;


		inline Colour GetPixel(const unsigned _x, const unsigned _y) const {
			return GetPixelByIndex(_x*width + _y);
		}

		inline Colour SetPixel(const unsigned _x, const unsigned _y, const Colour &_c) {
			SetPixelByIndex(_x*width + _y, _c);
		}

		Colour GetPixelUV(const float u, const float v) const {
			const unsigned int x = std::min((float)(width - 1), u * width);
			const unsigned int y = std::min((float)(height - 1), v * height);
			return GetPixel(x,y);
		}

		Colour GetUVBilinear(const float u, const float v) const {
			const float fx = std::min((float)(width - 1), u * width);
			const float fy = std::min((float)(height - 1), v * height);
			const unsigned x = (unsigned)fx;
			const unsigned y = (unsigned)fy;
			const unsigned x1 = std::min(x + 1, width - 1);
			const unsigned y1 = std::min(y + 1, height - 1);
			const Colour ya = Colour::Lerp(GetPixel(x, y), (x1, y), fx - (float)x);
			const Colour yb = Colour::Lerp(GetPixel(x, y1), GetPixel(x1, y1), fx - (float)x);
			return Colour::Lerp(ya, yb, fy - (float)y);
		}
		
		void SaveToImageFile(const char* name = "out.png", const bool gammaCorrect = true) const {
			const std::string sName = name;
			const unsigned dot = sName.find('.');
			const std::string format = sName.substr(dot+1);
			std::vector<uint32_t> p(width * height);
			for (unsigned int y = 0; y < height; ++y)
				for (unsigned int x = 0; x < width; ++x) {
					const Colour pix = GetPixel(x, y);
					float _r = pix.r;
					float _g = pix.g;
					float _b = pix.b;
					float _a = pix.a;
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
							const Colour col(data[r], data[g], data[b], data[a]);
							SetPixel(x, y, col);
						}
					}
					std::cout << std::endl << "Loaded: " << path;
				}
				else { std::cout << std::endl << "Could not load: " << path; }
			}
		}
};

class TextureRGBAFloat : public Texture {
	protected:
		std::vector<Colour> pixels;
	
	public:
		TextureRGBAFloat(const unsigned _x, const unsigned _y, const Colour &_c = Colour()) {
			width = _x;
			height = _y;
			pixels.resize(_x*_y, _c);
		}
	
		Colour GetPixelByIndex(const size_t _i) const override {
			return pixels[_i];
		}
	
		void SetPixelByIndex(const size_t _i, const Colour &_c) override {
			pixels[_i] = _c;
		}
	
		void Resize(const unsigned _x, const unsigned _y, const Colour &_c = Colour()) override {
			width = _x;
			height = _y;
			pixels.resize(_x*_y, _c);
		}
};