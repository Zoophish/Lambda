#pragma once
#include <iostream>
#include "Denoise.h"

LAMBDA_BEGIN

namespace PostProcessing {

	Denoise::Denoise() {
		device = oidn::newDevice(oidn::DeviceType::Default);
		device.commit();
	}

	void Denoise::SetData(const Texture *_colour, const Texture *_albedo, const Texture *_normal) {
		filter = device.newFilter("RT");
		filter.setImage("color",
			&((*_colour)[0]),
			oidn::Format::Float3,
			_colour->GetWidth(),
			_colour->GetHeight(),
			0,
			sizeof(Colour));
		if (_albedo) {
			filter.setImage("albedo",
				&((*_albedo)[0]),
				oidn::Format::Float3,
				_albedo->GetWidth(),
				_albedo->GetHeight(),
				0,
				sizeof(Colour));
		}
		if (_normal) {
			filter.setImage("normal",
				&((*_normal)[0]),
				oidn::Format::Float3,
				_normal->GetWidth(),
				_normal->GetHeight(),
				0,
				sizeof(Colour));
		}
		filter.set("hdr", true);
		filter.set("srgb", false);
	}

	void Denoise::Process(Texture *_texture) const {
		void *ptr = (float *)(&(*_texture)[0]);
		filter.setImage("output",
			&(_texture[0]),
			oidn::Format::Float3,
			_texture->GetWidth(),
			_texture->GetHeight(),
			0,
			sizeof(Colour));
		filter.commit();
		filter.execute();
		const char *errorMessage;
		if (device.getError(errorMessage) != oidn::Error::None)
			std::cout << "Error: " << errorMessage << std::endl;
	}

}

LAMBDA_END