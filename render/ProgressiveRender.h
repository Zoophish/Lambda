#pragma once
#include <future>
#include "Render.h"

LAMBDA_BEGIN

/*
	Progressively renders to outputImage texture.
		- Updates outputImage every sample.
		- Progressively increases resolution until target resolution is met at 1spp.
		- Stops rendering when renderDirective.spp is reached.
*/
class ProgressiveRender {
	public:
		/*
			The tile rendering method to be used
		*/
		TileRenderer tileRenderer;

		/*
			RGBA 32bit float texture that can be pointed to externally
			to use as a view texture.
			When updated, updateCallback() is called.
		*/
		Texture outputTexture;

		/*
			When called when output texture has been updated - so the applications
			'view' texture should be updated when this is called.
		*/
		std::function<void()> updateCallback;

		/*
			Output is updated every refreshRate number of samples.
		*/
		unsigned refreshRate = 1;

		ProgressiveRender(const RenderDirective &_renderDirective, TileRenderer _tileRenderer);

		/*
			Starts rendering on the render thread.
		*/
		void Start();

		/*
			Stops the render thread.
		*/
		void Stop();

		/*
			Pauses the render thread.
		*/
		//void Pause();

		/*
			Refreshes the output image.
		*/
		void Work();

		/*
			Sets the target number of threads to use.
		*/
		//void SetNumThreads(const unsigned _numThreads);

		/*
			Resizes output image.
		*/
		void Resize(const unsigned _w, const unsigned _h);

		/*
			Will ignore the directive's film pointer because ProgressiveRender renders to
			its own film.
		*/
		void SetRenderDirective(const RenderDirective &_directive);

		/*
			Returns main full res film reference (read only)
		*/
		const Film &GetFilm() const;

	protected:
		RenderDirective renderDirective;
		std::vector<Film> levelFilms; //Element 0 being the main film.
		std::vector<RenderMosaic> levelMosaics;
		std::thread renderThread;
		std::vector<std::future<void>> futures;

		void ResetLevels();

		void UpdateOutputTexture(const unsigned _level);

		void Waiter();
};

LAMBDA_END