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

//Temporary
namespace RenderCoordinator {

		static void ProcessTile(const RenderTile &_tile, const unsigned spp) {
			const Real xi = (Real)1 / _tile.film->filmData.GetWidth();
			const Real yi = (Real)1 / _tile.film->filmData.GetHeight();
			const Real xa = (Real)_tile.x / (Real)_tile.film->filmData.GetWidth();
			const Real ya = (Real)_tile.y / (Real)_tile.film->filmData.GetHeight();
			int percentage = 0;
			for (unsigned y = _tile.y; y < _tile.y + _tile.h; ++y) {
				const float p = (((float)y - (float)_tile.y) / (float)(_tile.h)) * 100.f;
				if (p >= percentage + 10.f) {
					percentage = p;
					std::cout << std::endl << percentage << '%';
				}
				for (unsigned x = _tile.x; x < _tile.x + _tile.w; ++x) {
					if (_tile.integrator->sampler->sampleShifter) {
						_tile.integrator->sampler->sampleShifter->SetPixelIndex(_tile.film->filmData.GetWidth(), _tile.film->filmData.GetHeight(), x, y);
					}
					_tile.integrator->sampler->SetSample(0);
					for (unsigned i = 0; i < spp; ++i) {
						const Real u = xi * ((Real)x + _tile.integrator->sampler->Get1D() - .5);
						const Real v = yi * ((Real)y + _tile.integrator->sampler->Get1D() - .5);
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