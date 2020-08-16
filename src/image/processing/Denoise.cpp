#pragma once
#include <iostream>
#include "Denoise.h"

LAMBDA_BEGIN

namespace PostProcessing {

	Denoise::Denoise() {
		
	}

	void Denoise::SetData(Texture *_colour, Texture *_albedo, Texture *_normal) {
		device = oidn::newDevice(oidn::DeviceType::Default);
		device.commit();
		filter = device.newFilter("RT");
 		filter.setImage("color",
			_colour->GetData(),
			oidn::Format::Float3,
			_colour->GetWidth(),
			_colour->GetHeight(),
			0,
			sizeof(Colour));
		if (_albedo) {
			filter.setImage("albedo",
				_albedo->GetData(),
				oidn::Format::Float3,
				_albedo->GetWidth(),
				_albedo->GetHeight(),
				0,
				sizeof(Colour));
		}
		if (_normal) {
			filter.setImage("normal",
				_normal->GetData(),
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
			_texture->GetData(),
			oidn::Format::Float3,
			_texture->GetWidth(),
			_texture->GetHeight(),
			0,
			sizeof(Colour));
		filter.commit();
		const char *errorMessage;
		if (device.getError(errorMessage) != oidn::Error::None)
			std::cout << "Error: " << errorMessage << std::endl;
		filter.execute();
		if (device.getError(errorMessage) != oidn::Error::None)
			std::cout << "Error: " << errorMessage << std::endl;
	}

}

LAMBDA_END