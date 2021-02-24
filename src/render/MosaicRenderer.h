#pragma once
#include <future>
#include "Render.h"


LAMBDA_BEGIN

class MosaicRenderer {
	public:
		RenderMosaic mosaic;
		TileRenderer tileRenderer;

		MosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer);

		virtual void Render() const = 0;
};

/*
	Uses OpenMP to exectute tile rendering in parallel.
*/
class OMPMosaicRenderer : public MosaicRenderer {
	public:
		unsigned nThreads;

		OMPMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer, const unsigned _nThreads = 4);

		void Render() const override;
};

/*
	Standard library tile renderer, works well on MSVC.
*/
class AsyncMosaicRenderer : public MosaicRenderer {
	public:

		AsyncMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer);

		void Render() const override;

	private:
		mutable std::vector<std::future<void>> futures;
};

/*
	Intel TBB threaded renderer.
*/
class TBBMosaicRenderer : public MosaicRenderer {
	public:
		
		TBBMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer);

		void Render() const override;
};

LAMBDA_END