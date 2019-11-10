#include <iostream>
#include "Texture.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "third_party\stb_image_write.h"
#define STB_IMAGE_IMPLEMENTATION    
#include "third_party\stb_image.h"

/*
	-------- Saving Utility Functions --------
*/

static std::string PathFormat(const std::string &_path) {
	const unsigned dot = _path.find('.');
	return _path.substr(dot + 1);
}

static void Gamma(float *_rgba) {
	_rgba[0] = std::powf(_rgba[0], GAMMA_POW);
	_rgba[1] = std::powf(_rgba[1], GAMMA_POW);
	_rgba[2] = std::powf(_rgba[2], GAMMA_POW);
}

static uint32_t PackChannels(const float *_rgba) {
	const int rI = std::max(0, std::min(255, (int)(_rgba[0] * 256)));
	const int gI = std::max(0, std::min(255, (int)(_rgba[1] * 256)));
	const int bI = std::max(0, std::min(255, (int)(_rgba[2] * 256)));
	const int aI = std::max(0, std::min(255, (int)(_rgba[3] * 256)));
	return (aI << 24) + (bI << 16) + (gI << 8) + rI;
}

/*
	Replace w/ switch logic?
*/
static bool WriteData(const uint32_t *_p, const char *_path, const unsigned _w, const unsigned _h) {
	const std::string format = PathFormat(_path);
	if (format == "png") { stbi_write_png(_path, _w, _h, 4, &_p[0], _w * 4); return true; }
	if (format == "jpg") { stbi_write_jpg(_path, _w, _h, 4, &_p[0], 100); return true; }
	if (format == "bmp") { stbi_write_bmp(_path, _w, _h, 4, &_p[0]); return true; }
	if (format == "tga") { stbi_write_tga(_path, _w, _h, 4, &_p[0]); return true; }
	std::cout << std::endl << "Did not save image: " << format << " is not a valid format.";
	return false;
}

/*
	---------- RGB32 ----------
*/

void texture_t<ColourFormat::RGB32>::SaveToImageFile(const char *_name, const bool _gammaCorrect) const {
	const std::string format = PathFormat(_name);
	std::unique_ptr<uint32_t[]> p(new uint32_t[width * height]);
	for (unsigned int y = 0; y < height; ++y)
		for (unsigned int x = 0; x < width; ++x) {
			const ColourFormat::RGB32 txl = GetPixelCoord(x, y);
			float rgba[4];
			rgba[0] = txl.r;
			rgba[1] = txl.g;
			rgba[2] = txl.b;
			rgba[3] = 1;
			if (_gammaCorrect) Gamma(rgba);
			p[(y * width) + x] = PackChannels(rgba);
		}
	WriteData(p.get(), _name, width, height);
}

void texture_t<ColourFormat::RGB32>::LoadImageFile(const char *_path, int _channels) {
	int file_width, file_height;
	if (stbi_info(_path, &file_width, &file_height, nullptr)) {
		Resize(file_width, file_height);
		std::cout << std::endl << "Loading " << _path;
		int w = width, h = height;
		const float *data = stbi_loadf(_path, &w, &h, &_channels, 0);
		if (data != nullptr) {
			for (unsigned y = 0; y < height; ++y) {
				for (unsigned x = 0; x < width; ++x) {
					const int r = width * y * _channels + x * _channels;
					const int g = _channels > 1 ? r + 1 : 0;
					const int b = _channels > 2 ? r + 2 : 0;
					const int a = _channels > 3 ? r + 3 : 0;
					SetPixelCoord(x, y, ColourFormat::RGB32(&data[r]));
				}
			}
			std::cout << std::endl << "Loaded image.";
		}
		else { std::cout << std::endl << "Could not load: " << _path; }
	}
}

/*
	---------- RGBA32 ----------
*/

void texture_t<ColourFormat::RGBA32>::SaveToImageFile(const char *_name, const bool _gammaCorrect) const {
	const std::string format = PathFormat(_name);
	std::unique_ptr<uint32_t[]> p(new uint32_t[width * height]);
	for (unsigned int y = 0; y < height; ++y)
		for (unsigned int x = 0; x < width; ++x) {
			const ColourFormat::RGBA32 txl = GetPixelCoord(x, y);
			float rgba[4];
			rgba[0] = txl.r;
			rgba[1] = txl.g;
			rgba[2] = txl.b;
			rgba[3] = txl.a;
			if (_gammaCorrect) Gamma(rgba);
			p[(y * width) + x] = PackChannels(rgba);
		}
	WriteData(p.get(), _name, width, height);
}

void texture_t<ColourFormat::RGBA32>::LoadImageFile(const char *_path, int _channels) {
	int file_width, file_height;
	if (stbi_info(_path, &file_width, &file_height, nullptr)) {
		Resize(file_width, file_height);
		std::cout << std::endl << "Loading " << _path;
		int w = width, h = height;
		const float *data = stbi_loadf(_path, &w, &h, &_channels, 0);
		if (data != nullptr) {
			for (unsigned y = 0; y < height; ++y) {
				for (unsigned x = 0; x < width; ++x) {
					const int r = width * y * _channels + x * _channels;
					const int g = _channels > 1 ? r + 1 : 0;
					const int b = _channels > 2 ? r + 2 : 0;
					const int a = _channels > 3 ? r + 3 : 0;
					SetPixelCoord(x, y, ColourFormat::RGBA32(&data[r]));
				}
			}
			std::cout << std::endl << "Loaded image.";
		}
		else { std::cout << std::endl << "Could not load: " << _path; }
	}
}