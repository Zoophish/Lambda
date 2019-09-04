#pragma once
#include <list>
#include <integrators/Integrator.h>
#include <camera/Film.h>
#include <camera/Camera.h>

struct RenderTile {
	Film *film;
	std::unique_ptr<Integrator> integrator;
	std::unique_ptr<Sampler> sampler;
	std::unique_ptr<SampleShifter> sampleShifter;
	Camera *camera;
	Scene *scene;
	unsigned x, y, w, h, spp;
};

//class TileRenderer {
//	public:
//		RenderTile *tile;
//
//		virtual void Render() const = 0;
//};

//

namespace TileRenderers {

	static void UniformSpp(const RenderTile *_tile) {
		const unsigned w = _tile->film->filmData.GetWidth();
		const unsigned h = _tile->film->filmData.GetHeight();
		const Real xi = (Real)1 / w;
		const Real yi = (Real)1 / h;
		for (unsigned y = _tile->y; y < _tile->y + _tile->h; ++y) {
			for (unsigned x = _tile->x; x < _tile->x + _tile->w; ++x) {
				if (_tile->integrator->sampler->sampleShifter)
					_tile->integrator->sampler->sampleShifter->SetPixelIndex(w, h, x, y);
				_tile->integrator->sampler->SetSample(0);
				for (unsigned i = 0; i < _tile->spp; ++i) {
					const Real u = xi * ((Real)x + _tile->integrator->sampler->Get1D() - .5);
					const Real v = yi * ((Real)y + _tile->integrator->sampler->Get1D() - .5);
					const Ray r = _tile->camera->GenerateRay(u, v);
					const Spectrum sample = _tile->integrator->Li(r, *_tile->scene);
					_tile->film->AddSample(sample, x, y);
					_tile->integrator->sampler->NextSample();
				}
			}
		}
	}

	//Will render until max spp reached, but stops rendering pixels that are suitably converged.
	static void ConvergeAndStop(const RenderTile *_tile) {
		const unsigned w = _tile->film->filmData.GetWidth();
		const unsigned h = _tile->film->filmData.GetHeight();
		const Real xi = (Real)1 / w;
		const Real yi = (Real)1 / h;

		const unsigned s = _tile->w * _tile->h;
		std::vector<bool> completionBuffer(s, false);
		unsigned sampleIndex = 0, jump = 31, completions = 0;
		while (sampleIndex < _tile->spp) {
			if (completions < s) {
				for (unsigned y = _tile->y; y < _tile->y + _tile->h; ++y) {
					for (unsigned x = _tile->x; x < _tile->x + _tile->w; ++x) {
						if (!completionBuffer[(y - _tile->y) * _tile->w + x - _tile->x]) {
							if (_tile->integrator->sampler->sampleShifter)
								_tile->integrator->sampler->sampleShifter->SetPixelIndex(w, h, x, y);
							const Real pre = _tile->film->filmData.GetPixelCoord(x, y).spec.y_fast();
							_tile->integrator->sampler->SetSample(sampleIndex);
							for (unsigned i = 0; i < jump; ++i) {
								const Real u = xi * ((Real)x + _tile->integrator->sampler->Get1D() - .5);
								const Real v = yi * ((Real)y + _tile->integrator->sampler->Get1D() - .5);
								const Ray r = _tile->camera->GenerateRay(u, v);
								const Spectrum sample = _tile->integrator->Li(r, *_tile->scene);
								_tile->film->AddSample(sample, x, y);
								_tile->integrator->sampler->NextSample();
							}
							const Real dif = _tile->film->filmData.GetPixelCoord(x, y).spec.y_fast() - pre;
							if (dif * dif < .004) {
								completionBuffer[(y - _tile->y) * _tile->w + x - _tile->x] = true;
								completions++;
							}
						}
					}
				}
			}
			sampleIndex += jump;
		}
		std::cout << std::endl << "Completions: " << completions;
	}
}