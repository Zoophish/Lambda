#include "Render.h"

LAMBDA_BEGIN

RenderMosaic::RenderMosaic(const RenderDirective &_directive) {
	const unsigned w = _directive.film->filmData.GetWidth();
	const unsigned h = _directive.film->filmData.GetHeight();
	const unsigned rX = w % _directive.tileSizeX, rY = h % _directive.tileSizeY;
	nX = (w / _directive.tileSizeX) + (rX > 0 ? 1 : 0);
	nY = (h / _directive.tileSizeY) + (rY > 0 ? 1 : 0);
	const bool padX = rX > 0, padY = rY > 0;
	tiles.reset(new RenderTile[nX * nY]);
	for (unsigned y = 0; y < nY; ++y) {
		for (unsigned x = 0; x < nX; ++x) {
			RenderTile &t = tiles[y * nX + x];
			t.camera = _directive.camera;
			t.film = _directive.film;
			t.integrator.reset(_directive.integrator->clone()); //Copy per thread
			t.sampler.reset(_directive.sampler->clone()); //
			t.sampleShifter.reset(new SampleShifter(*_directive.sampleShifter)); //
			t.sampler->sampleShifter = t.sampleShifter.get();
			t.integrator->sampler = t.sampler.get();
			t.scene = _directive.scene;
			t.spp = _directive.spp;
			if (padX && x == nX - 1) t.w = rX;
			else t.w = _directive.tileSizeX;
			if (padY && y == nY - 1) t.h = rY;
			else t.h = _directive.tileSizeY;
			t.x = x * _directive.tileSizeX;
			t.y = y * _directive.tileSizeY;
		}
	}
}

void TileRenderers::UniformSpp(const RenderTile *_tile) {
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
				const Ray r = _tile->camera->GenerateRay(u, v, *_tile->sampler);
				const Spectrum sample = _tile->integrator->Li(r, *_tile->scene);
				_tile->film->AddSample(sample, x, y);
				_tile->integrator->sampler->NextSample();
			}
		}
	}
}

void TileRenderers::MaxSpp(const RenderTile *_tile) {
	const unsigned w = _tile->film->filmData.GetWidth();
	const unsigned h = _tile->film->filmData.GetHeight();
	const Real xi = (Real)1 / w;
	const Real yi = (Real)1 / h;

	const unsigned s = _tile->w * _tile->h;
	std::vector<bool> completionBuffer(s, false);
	unsigned sampleIndex = 0, jump = 64, completions = 0;
	while (sampleIndex < _tile->spp) {
		if (completions < s) {
			for (unsigned y = _tile->y; y < _tile->y + _tile->h; ++y) {
				for (unsigned x = _tile->x; x < _tile->x + _tile->w; ++x) {
					if (!completionBuffer[(y - _tile->y) * _tile->w + x - _tile->x]) {
						if (_tile->integrator->sampler->sampleShifter)
							_tile->integrator->sampler->sampleShifter->SetPixelIndex(w, h, x, y);
						const Real pre = _tile->film->filmData.GetPixelCoord(x, y).spectrum.y();
						_tile->integrator->sampler->SetSample(sampleIndex);
						for (unsigned i = 0; i < jump; ++i) {
							const Real u = xi * ((Real)x + _tile->integrator->sampler->Get1D() - .5);
							const Real v = yi * ((Real)y + _tile->integrator->sampler->Get1D() - .5);
							const Ray r = _tile->camera->GenerateRay(u, v, *_tile->sampler);
							const Spectrum sample = _tile->integrator->Li(r, *_tile->scene);
							_tile->film->AddSample(sample, x, y);
							_tile->integrator->sampler->NextSample();
						}
						const Real dif = (_tile->film->filmData.GetPixelCoord(x, y).spectrum.y() - pre) / (Real)sampleIndex;
						if (dif * dif < .05) {
							completionBuffer[(y - _tile->y) * _tile->w + x - _tile->x] = true;
							completions++;
						}
					}
				}
			}
		}
		sampleIndex += jump;
	}
	if (completions < s) std::cout << std::endl << "Undersampled: " << s - completions << " pixels.";
}

LAMBDA_END