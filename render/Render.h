#pragma once
#include "RenderTile.h"

struct RenderDirective {
	Film *film;
	Scene *scene;
	Camera *camera;
	Integrator *integrator;
	Sampler *sampler;
	SampleShifter *sampleShifter;
	unsigned tileSizeX, tileSizeY, spp;
};

struct RenderMosaic {
	unsigned nX, nY;
	std::vector<RenderTile> tiles;

	RenderTile &operator()(const unsigned _x, const unsigned _y) {
		return tiles[_y * nX + _x];
	}
};

static RenderMosaic CreateMosaic(const RenderDirective &_directive) {
	const unsigned w = _directive.film->filmData.GetWidth();
	const unsigned h = _directive.film->filmData.GetHeight();
	const unsigned rX = w % _directive.tileSizeX, rY = h % _directive.tileSizeY;
	const unsigned nX = (w / _directive.tileSizeX) + (rX > 0 ? 1 : 0);
	const unsigned nY = (h / _directive.tileSizeY) + (rY > 0 ? 1 : 0);
	const bool padX = rX > 0, padY = rY > 0;
	RenderMosaic out;
	out.nX = nX; out.nY = nY;
	out.tiles.resize(nX * nY);
	for (unsigned y = 0; y < nY; ++y) {
		for (unsigned x = 0; x < nX; ++x) {
			RenderTile &t = out.tiles[y * nX + x];
			t.camera = _directive.camera;
			t.film = _directive.film;
			t.integrator.reset(_directive.integrator->clone()); //Each tile has own copy - thread independence.
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
	return out;
}