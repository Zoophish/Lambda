#pragma once
#include <string>
#include "Texture.h"
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include <stb_image/stb_image_write.h>
#define STB_IMAGE_IMPLEMENTATION    
#include <stb_image/stb_image.h>


/*
	-------- Saving Utility Functions --------
*/

static std::string PathFormat(const std::string &_path) {
	const unsigned dot = _path.find('.');
	return _path.substr(dot + 1);
}

static void Gamma(float *_rgba) {
	_rgba[0] = std::powf(_rgba[0], lambda::GAMMA_POW);
	_rgba[1] = std::powf(_rgba[1], lambda::GAMMA_POW);
	_rgba[2] = std::powf(_rgba[2], lambda::GAMMA_POW);
}

static uint32_t PackChannels(const float *_rgba) {
	const int rI = std::max(0, std::min(255, (int)(_rgba[0] * 256)));
	const int gI = std::max(0, std::min(255, (int)(_rgba[1] * 256)));
	const int bI = std::max(0, std::min(255, (int)(_rgba[2] * 256)));
	const int aI = std::max(0, std::min(255, (int)(_rgba[3] * 256)));
	return (aI << 24) + (bI << 16) + (gI << 8) + rI;
}

static bool WriteData(const uint32_t *_p, const char *_path, const unsigned _w, const unsigned _h) {
	const std::string format = PathFormat(_path);
	if (format == "png") { stbi_write_png(_path, _w, _h, 4, &_p[0], _w * 4); return true; }
	if (format == "jpg") { stbi_write_jpg(_path, _w, _h, 4, &_p[0], 100); return true; }
	if (format == "bmp") { stbi_write_bmp(_path, _w, _h, 4, &_p[0]); return true; }
	if (format == "tga") { stbi_write_tga(_path, _w, _h, 4, &_p[0]); return true; }
	printf("\nDid not save texture: %s is not a supported format.", format.c_str());
	return false;
}


LAMBDA_BEGIN

Texture::Texture(const unsigned _w, const unsigned _h, const Colour &_c) : texture_t<Colour>(_w, _h, _c) {}

Texture Texture::Copy(const Texture &_texture) {
	Texture copy;
	copy.width = _texture.width;
	copy.height = _texture.height;
	copy.encoding = _texture.encoding;
	copy.data.reset(new Colour[_texture.width * _texture.height]);
	memcpy(&copy.data[0], &_texture.data[0], sizeof(Colour) * _texture.width * _texture.height);
	return copy;
}

bool Texture::GetFileInfo(const char *_path, int *_width, int *_height, int *_channels) {
	return stbi_info(_path, _width, _height, _channels);
}

bool Texture::GetMemoryInfo(const void *_src, const int _size, int *_w, int *_h, int *_channels) {
	return stbi_info_from_memory((const stbi_uc *)_src, _size, _w, _h, _channels);
}

void Texture::SaveToImageFile(const char *_path, const bool _gammaCorrect, const bool _alpha) const {
	const std::string format = PathFormat(_path);
	std::unique_ptr<uint32_t[]> p(new uint32_t[width * height]);
	for (unsigned y = 0; y < height; ++y)
		for (unsigned x = 0; x < width; ++x) {
			const Colour &txl = GetPixelCoord(x, y);
			float rgba[4];
			rgba[0] = txl.r;
			rgba[1] = txl.g;
			rgba[2] = txl.b;
			rgba[3] = _alpha ? txl.a : 1;
			if (_gammaCorrect) Gamma(rgba);
			p[(y * width) + x] = PackChannels(rgba);
		}
	WriteData(p.get(), _path, width, height);
}

void Texture::ParseData(float *_data, const int _channels) {
	for (unsigned y = 0; y < height; ++y) {
		for (unsigned x = 0; x < width; ++x) {
			const int r = width * y * _channels + x * _channels;
			const int g = _channels > 1 ? r + 1 : 0;
			const int b = _channels > 2 ? r + 2 : 0;
			const int a = _channels > 3 ? r + 3 : 0;
			SetPixelCoord(x, y, Colour(&_data[r], _channels > 3));
		}
	}
}

void Texture::LoadImageFile(const char *_path, int _channels) {
	int file_width, file_height;
	if (stbi_info(_path, &file_width, &file_height, nullptr)) {
		Resize(file_width, file_height);
		printf("\nLoading %s", _path);
		int w = width, h = height;
		float *data = stbi_loadf(_path, &w, &h, &_channels, 0);
		if (data) {
			ParseData(data, _channels);
			printf("\nLoaded texture.");
		}
		else { 
			printf("\nCould not load: %s", _path);
		}
		stbi_image_free(data);
	}
	else printf("\nImage file not found: %s", _path);
}

void Texture::LoadFromMemory(const void *_src, const int _size, const int _channels) {
	int w, h;
	printf("\nLoading texture: %p", _src);
	float *data = stbi_loadf_from_memory((const stbi_uc *)_src, _size, &w, &h, nullptr, _channels);
	if (data) {
		ParseData(data, _channels);
		printf("\nLoaded texture.");
	}
	else {
		printf("\nCould not load from memory address: %p", _src);
	}
	stbi_image_free(data);
}

LAMBDA_END