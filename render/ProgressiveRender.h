#pragma once
#include "Render.h"

LAMBDA_BEGIN

/*
	Progressively renders to outputImage texture.
		- Updates outputImage every sample.
		- Stops rendering when renderDirective.spp is reached.
*/
class ProgressiveRender {
	public:
		TileRenderer tileRenderer;
		Texture outputTexture;
		std::function<void()> updateCallback;	//Called when output texture has been updated
		const Film &film = levelFilms[0];
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