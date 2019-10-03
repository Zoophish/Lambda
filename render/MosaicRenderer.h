#pragma once
#include <omp.h>
#include <thread>
#include "Render.h"

class MosaicRenderer {
	public:
		RenderMosaic mosaic;

		virtual void Render() const = 0;
};

class ThreadedMosaicRenderer : public MosaicRenderer {
	public:
		unsigned nThreads;
		void (*tileRenderer)(const RenderTile*);

		ThreadedMosaicRenderer(const RenderDirective &_directive, void* _tileRenderer, const unsigned _nThreads = 4) {
			mosaic = CreateMosaic(_directive);
			tileRenderer = reinterpret_cast<void(*)(const RenderTile*)>(_tileRenderer);
			nThreads = _nThreads;
		}

		void Render() const override {
			std::unique_ptr<std::thread[]> t(new std::thread[nThreads]);
			const unsigned n = mosaic.tiles.size() / nThreads, remainder = mosaic.tiles.size() % nThreads;
			int p = 0;
			for (unsigned i = 0; i < n + 1; ++i) {
				const unsigned workers = i < n ? nThreads : remainder;
				for (unsigned j = 0; j < workers; ++j) {
					t[j] = std::thread(tileRenderer, &mosaic.tiles[i * nThreads + j]);
				}
				for (unsigned j = 0; j < workers; ++j) {
					t[j].join();
					const Real pt = ((Real)(i * nThreads + j) / mosaic.tiles.size()) * 100.f;
					if (pt > p + 1) {
						p = pt;
						std::cout << std::endl << p << '%';
					}
				}
			}
		}

		void RenderOmp() const {
			int p = 0;
			#pragma omp parallel for
			for (int i = 0; i < mosaic.tiles.size(); ++i) {
				tileRenderer(&mosaic.tiles[i]);
				const Real pt = 100.f * (Real)i / mosaic.tiles.size();
				if (pt > p + 1) {
					p = pt;
					std::cout << std::endl << p << '%';
				}
			}
		}
};