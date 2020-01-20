#pragma once
#include <future>
#include "Render.h"

class MosaicRenderer {
	public:
		RenderMosaic mosaic;
		void (*tileRenderer)(const RenderTile *);

		MosaicRenderer(const RenderDirective &_directive, void *_tileRenderer) {
			mosaic = RenderMosaic(_directive);
			tileRenderer = reinterpret_cast<void(*)(const RenderTile *)>(_tileRenderer);
		}

		virtual void Render() const = 0;
};

class OMPMosaicRenderer : public MosaicRenderer {
	public:
		unsigned nThreads;

		OMPMosaicRenderer(const RenderDirective &_directive, void* _tileRenderer, const unsigned _nThreads = 4)
			: MosaicRenderer(_directive, _tileRenderer) {
			nThreads = _nThreads;
		}

		void Render() const override;
};

class AsyncMosaicRenderer : public MosaicRenderer {
	public:

		AsyncMosaicRenderer(const RenderDirective &_directive, void *_tileRenderer)
			: MosaicRenderer(_directive, _tileRenderer) {}

		void Render() const override;

	private:
		mutable std::vector<std::future<void>> futures;
};