#pragma once
#include <integrators/Integrator.h>
#include <camera/Film.h>
#include <camera/Camera.h>

struct RenderTile {
	Film *film;
	Integrator *integrator;
	Camera *camera;
	Scene *scene;
	unsigned x, y, w, h;
};

namespace RenderCoordinator {

		static void ProcessTile(const RenderTile &_tile, const unsigned spp) {
			const Real xi = (Real)1 / _tile.film->filmData.GetWidth();
			const Real yi = (Real)1 / _tile.film->filmData.GetHeight();
			const Real xa = (Real)_tile.x / _tile.film->filmData.GetWidth();
			const Real ya = (Real)_tile.y / _tile.film->filmData.GetHeight();
			for (unsigned y = _tile.y; y < _tile.h; ++y) {
				for (unsigned x = _tile.x; x < _tile.w; ++x) {
					if (_tile.integrator->sampler->sampleShifter) {
						_tile.integrator->sampler->sampleShifter->SetPixelIndex(_tile.film->filmData.GetWidth(), _tile.film->filmData.GetHeight(), x, y);
					}
					for (unsigned i = 0; i < spp; ++i) {
						const Real u = xa + xi * ((Real)x + _tile.integrator->sampler->Get1D() - .5);
						const Real v = ya + yi * ((Real)y + _tile.integrator->sampler->Get1D() - .5);
						const Ray r = _tile.camera->GenerateRay(u, v);
						const Spectrum sample = _tile.integrator->Li(r, *_tile.scene);
						_tile.film->AddSample(sample, x, y);
						_tile.integrator->sampler->NextSample();
					}
				}
			}
		}

		static RenderTile MakeTile(const unsigned _xi, const unsigned _yi, const unsigned _ts) {

		}
};