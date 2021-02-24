#pragma once
#include <iostream>
#include <omp.h>
#include <tbb/parallel_for.h>
#include "MosaicRenderer.h"

LAMBDA_BEGIN

MosaicRenderer::MosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer) {
	mosaic = RenderMosaic(_directive);
	tileRenderer = _tileRenderer;
}



OMPMosaicRenderer::OMPMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer, const unsigned _nThreads)
	: MosaicRenderer(_directive, _tileRenderer)
{
	nThreads = _nThreads;
}

void OMPMosaicRenderer::Render() const {
	const unsigned s = mosaic.nX * mosaic.nY;
	int p = 0;
	std::cout << std::endl;
	omp_set_num_threads(nThreads);
	#pragma omp parallel for// schedule(dynamic)
	for (int i = 0; i < s; ++i) {
		tileRenderer(&mosaic.tiles[i]);
		const Real pt = 100.f * (Real)i / (Real)s;
		if (pt > p + 1) {
			p = pt;
			std::cout << "\r" << p << '%';
		}
	}
}



AsyncMosaicRenderer::AsyncMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer)
	: MosaicRenderer(_directive, _tileRenderer) {}

void AsyncMosaicRenderer::Render() const {
	const unsigned s = mosaic.nX * mosaic.nY;
	for (unsigned i = 0; i < s; ++i) {
		futures.push_back(std::async(std::launch::async, tileRenderer, &mosaic.tiles[i]));
	}
	int p = 0;
	std::cout << std::endl;
	for (unsigned i = 0; i < s; ++i) {
		futures[i].get();
		const Real pt = 100.f * (Real)i / (Real)s;
		if (pt > p + 1) {
			p = pt;
			std::cout << "\r" << p << '%';
		}
	}
}



TBBMosaicRenderer::TBBMosaicRenderer(const RenderDirective &_directive, TileRenderer _tileRenderer)
	: MosaicRenderer(_directive, _tileRenderer) {}

void TBBMosaicRenderer::Render() const {
	const unsigned s = mosaic.nX * mosaic.nY;
	int p = 0;
	tbb::parallel_for(tbb::blocked_range<int>(0, s), [&](tbb::blocked_range<int> r) {

		for (int i = r.begin(); i < r.end(); ++i) {
			tileRenderer(&mosaic.tiles[i]);
			const Real pt = 100.f * (Real)i / (Real)s;
			if (pt > p + 1) {
				p = pt;
				std::cout << "\r" << p << '%';
			}
		}

	});
}

LAMBDA_END