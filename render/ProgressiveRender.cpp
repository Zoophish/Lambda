#include <future>
#include "ProgressiveRender.h"

LAMBDA_BEGIN

ProgressiveRender::ProgressiveRender(const RenderDirective &_renderDirective, TileRenderer _tileRenderer) {
	outputTexture = Texture(_renderDirective.film->filmData.GetWidth(), _renderDirective.film->filmData.GetHeight());
	SetRenderDirective(_renderDirective);
}

void ProgressiveRender::Start() {
	renderThread = std::thread(&ProgressiveRender::Work, this);
}

void ProgressiveRender::Stop() {
	renderThread.~thread();
}

void ProgressiveRender::Work() {
	ResetLevels();
	const unsigned numLevels = levelFilms.size();
	for (unsigned level = numLevels - 1; level > 0; --level) {
		const unsigned nTiles = levelMosaics[level].nX * levelMosaics[level].nY;
		futures.resize(nTiles);
		for (unsigned j = 0; j < nTiles; ++j) {
			futures[j] = std::async(std::launch::async, tileRenderer, &levelMosaics[level].tiles[j]);
		}
		for (auto &it : futures) {
			it.get();
		}
		UpdateOutputTexture(level);
	}
	const unsigned nTiles = levelMosaics[0].nX * levelMosaics[0].nY;
	futures.resize(nTiles);
	for (unsigned spp = 0; spp < renderDirective.spp; ++spp) {
		for (unsigned j = 0; j < nTiles; ++j) {
			futures[j] = std::async(std::launch::async, tileRenderer, &levelMosaics[0].tiles[j]);
		}
		UpdateOutputTexture(0);
	}
}

void ProgressiveRender::Resize(const unsigned _w, const unsigned _h) {
	const float s = std::min(_w, _h);
	const unsigned nLevels = std::floor(std::log2(s));
	levelFilms.resize(nLevels);
	levelMosaics.resize(nLevels);
	unsigned tw = _w, th = _h;
	for (unsigned i = 0; i < nLevels; ++i) {
		levelFilms[i].filmData.Resize(tw, th);
		levelMosaics[i] = RenderMosaic(renderDirective);
		tw = std::floor((float)tw / 2.f);
		th = std::floor((float)th / 2.f);
	}
	Work();
}

void ProgressiveRender::SetRenderDirective(const RenderDirective &_directive) {
	renderDirective = _directive;
	renderDirective.spp = refreshRate;
	Work();
}

void ProgressiveRender::ResetLevels() {
	const FilmPixel fp;
	for (auto &it : levelFilms) {
		it.filmData.Resize(it.filmData.GetWidth(), it.filmData.GetHeight(), fp);
	}
}

void ProgressiveRender::UpdateOutputTexture(const unsigned _level) {
	const unsigned w = outputTexture.GetWidth(), h = outputTexture.GetHeight();
	const Real xr = (Real)levelFilms[_level].filmData.GetWidth() / (Real)w;
	const Real yr = (Real)levelFilms[_level].filmData.GetHeight() / (Real)h;
	for (unsigned y = 0; y < h; ++y) {
		for (unsigned x = 0; x < w; ++x) {
			const unsigned xc = std::floor((Real)x * xr);
			const unsigned yc = std::floor((Real)y * yr);
			Colour &c = outputTexture.GetPixelCoord(x, y);
			levelFilms[_level].filmData.GetPixelCoord(xc, yc).ToRGB(&c);
		}
	}
	updateCallback();
}

LAMBDA_END