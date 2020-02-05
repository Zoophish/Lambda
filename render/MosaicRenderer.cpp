#include <omp.h>
#include "MosaicRenderer.h"

LAMBDA_BEGIN

void OMPMosaicRenderer::Render() const {
	const unsigned s = mosaic.nX * mosaic.nY;
	int p = 0;
	std::cout << std::endl;
	omp_set_num_threads(nThreads);
	#pragma omp parallel for
	for (int i = 0; i < s; ++i) {
		tileRenderer(&mosaic.tiles[i]);
		const Real pt = 100.f * (Real)i / (Real)s;
		if (pt > p + 1) {
			p = pt;
			std::cout << "\r" << p << '%';
		}
	}
}

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

LAMBDA_END