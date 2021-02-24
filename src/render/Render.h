#pragma once
#include <integrators/Integrator.h>
#include <sampling/SampleShifter.h>
#include <camera/Film.h>
#include <camera/Camera.h>

LAMBDA_BEGIN

/*
	Bundles necassary information together needed to produce a render.
*/
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

//class TileRenderer {
//	public:
//		RenderTile *tile;
//
//		virtual void Render(const RenderTile &_tile) = 0;
//};

typedef void (*TileRenderer)(const RenderTile *);

struct RenderMosaic {
	std::vector<RenderTile> tiles;
	unsigned nX, nY;


	RenderMosaic();

	RenderMosaic(const RenderDirective &_directive);
};


namespace TileRenderers {

	/*
		Adds conribution to tile using render directive's spp parameter.
	*/
	void UniformSpp(const RenderTile *_tile);

	/*
		Adds single sample contribution to tile.
	*/
	void UniformIncrement(const RenderTile *_tile);

	//class UniformTileRenderer : public TileRenderer {
	//	public:
	//
	//		void Render(const RenderTile &_tile) override;
	//};
	//
	//class AdaptiveTileRenderer : public TileRenderer {
	//	public:
	//		AdaptiveTileRenderer(const RenderDirective &_directive);
	//
	//		void Render(const RenderTile &_tile) override;
	//
	//	private:
	//		Film film2;
	//		TextureR32 varianceBuffer;
	//		unsigned rate;
	//
	//		/*
	//			Calculates variance estimate by comparing films.
	//		*/
	//		Real PixelVariance(const unsigned _x, const unsigned _y) const;
	//};
}

LAMBDA_END