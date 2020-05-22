#include "PostProcessing.h"

LAMBDA_BEGIN

namespace PostProcessing {

	void PostProcessStack::Process(Texture *_texture) const {
		for (auto &it : stack) {
			it.Process(_texture);
		}
	}

	ConvolutionFilter::ConvolutionFilter(Texture *_kernel, const Real _intensity) {
		kernel = _kernel;
		intensity = _intensity;
	}

	void ConvolutionFilter::Process(Texture *_texture) const {
		const unsigned w = _texture->GetWidth(), h = _texture->GetHeight();
		Texture tmp = Texture::Copy(*_texture);
		for (unsigned y = 0; y < h; ++y) {
			for (unsigned x = 0; x < w; ++x) {
				const Colour sum = Filter(x, y, &tmp);
				_texture->SetPixelCoord(x,y, sum);
			}
		}
	}

	Colour ConvolutionFilter::Filter(unsigned _x, unsigned _y, Texture *_target) const {
		const unsigned w = _target->GetWidth();
		const unsigned h = _target->GetHeight();
		const unsigned kw = kernel->GetWidth();
		const unsigned kh = kernel->GetHeight();
		const unsigned kw2 = kw / 2;
		const unsigned kh2 = kh / 2;
		Colour sum(0.f);
		for (unsigned y = 0; y < kh; ++y) {
			for (unsigned x = 0; x < kw; ++x) {
				const Colour &kc = kernel->GetPixelCoord(x, y);
				const Colour &tc = _target->GetPixelCoord(maths::Clamp(_x + x - kw2, 0u, w - 1), maths::Clamp(_y + y - kh2, 0u, h - 1));
				sum += kc * intensity * tc;
			}
		}
		return sum;
	}

}

LAMBDA_END