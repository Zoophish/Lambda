#pragma once
#include <future>
#include "Render.h"

class MosaicRenderer {
	public:
		RenderMosaic mosaic;
		TileRenderer tileRenderer;

		MosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer) {
			mosaic = RenderMosaic(_directive);
			tileRenderer = _tileRenderer;
		}

		virtual void Render() const = 0;
};

/*
	Uses OpenMP to exectute tile rendering in parallel.
*/
class OMPMosaicRenderer : public MosaicRenderer {
	public:
		unsigned nThreads;

		OMPMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer, const unsigned _nThreads = 4)
			: MosaicRenderer(_directive, _tileRenderer) {
			nThreads = _nThreads;
		}

		void Render() const override;
};

/*
	Portable parallel tile renderer, but performance may between compilers.
*/
class AsyncMosaicRenderer : public MosaicRenderer {
	public:

		AsyncMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer)
			: MosaicRenderer(_directive, _tileRenderer) {}

		void Render() const override;

	private:
		mutable std::vector<std::future<void>> futures;
};