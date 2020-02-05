#pragma once
#include <integrators/Integrator.h>
#include <sampling/SampleShifter.h>
#include <camera/Film.h>
#include <camera/Camera.h>

LAMBDA_BEGIN

struct RenderDirective {
	Film *film;
	Scene *scene;
	Camera *camera;
	Integrator *integrator;
	Sampler *sampler;
	SampleShifter *sampleShifter;
	unsigned tileSizeX, tileSizeY, spp;
};

struct RenderTile {
	Film *film;
	std::unique_ptr<Integrator> integrator;
	std::unique_ptr<Sampler> sampler;
	std::unique_ptr<SampleShifter> sampleShifter;
	Camera *camera;
	Scene *scene;
	unsigned x, y, w, h, spp;
};

typedef void (*TileRenderer)(const RenderTile *);

struct RenderMosaic {
	unsigned nX, nY;
	std::unique_ptr<RenderTile[]> tiles;

	RenderMosaic() {}

	RenderMosaic(const RenderDirective &_directive);
};


namespace TileRenderers {

	void UniformSpp(const RenderTile *_tile);

	/*
		Will render until max spp reached, but stops rendering pixels that are suitably converged.
	*/
	void MaxSpp(const RenderTile *_tile);
}

LAMBDA_END