#pragma once
#include <utility/Concurrency.h>
#include "Render.h"

LAMBDA_BEGIN

struct TileTaskPackage {
	RenderTile *tile;

	void Work() {
		TileRenderers::UniformIncrement(tile);
	}
};

class ProgressiveRender {
	public:
		TileRenderer tileRenderer;
		Texture outputTexture;
		void(*updateCallback)();

		ProgressiveRender(const RenderDirective &_renderDirective);

		void Init();

		void Stop();

		void Clear();

	private:
		ThreadPool threadPool;
		RenderDirective renderDirective;
		RenderMosaic renderMosaic;
		std::vector<std::shared_ptr<Task>> tileTasks;
		std::vector<TileTaskPackage> tileTaskPackages;
		bool isRunning;
		
		void RunPass();

		void UpdateOutputTexture();
};

LAMBDA_END