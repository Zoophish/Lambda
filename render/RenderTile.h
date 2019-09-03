#pragma once
#include <integrators/Integrator.h>
#include <camera/Film.h>
#include <camera/Camera.h>

struct RenderTile {
	Film *film;
	Integrator *integrator;
	Camera *camera;
	Scene *scene;
	unsigned x, y, w, h, spp;
};

namespace TileRenderers {

	static void RenderTileUniformSpp(const RenderTile *_tile) {
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

}