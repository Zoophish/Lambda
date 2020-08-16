#pragma once
#include <OpenImageDenoise/oidn.hpp>
#include "PostProcessing.h"

LAMBDA_BEGIN

namespace PostProcessing {

	class Denoise : public PostProcess {
		public:
			Denoise();

			/*
				Set image data for denoise filter to use. Colour is required, albedo & normal are optional, but
				yield better results.
			*/
			void SetData(const Texture *_colour, const Texture *_albedo = nullptr, const Texture *_normal = nullptr);

			/*
				Apply denoising and save to _texture.
			*/
			void Process(Texture *_texture) const override;

		private:
			mutable oidn::DeviceRef device;
			mutable oidn::FilterRef filter;
	};

}

LAMBDA_END