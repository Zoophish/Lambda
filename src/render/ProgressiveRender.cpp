#include "ProgressiveRender.h"

LAMBDA_BEGIN

using SharedTask = std::shared_ptr<Task>;

ProgressiveRender::ProgressiveRender(const RenderDirective &_renderDirective) {
	renderDirective = _renderDirective;
	outputTexture = Texture(renderDirective.film->filmData.GetWidth(), renderDirective.film->filmData.GetHeight());
	updateCallback = nullptr;
}

void ProgressiveRender::Init() {
	if (!isRunning) { // prevent multiple initialisation
		isRunning = true;

		renderMosaic = RenderMosaic(renderDirective);

		tileTaskPackages.clear();
		const unsigned numTiles = renderMosaic.tiles.size();
		tileTaskPackages.reserve(numTiles);
		for (unsigned i = 0; i < numTiles; ++i) {
			tileTaskPackages.push_back({ &renderMosaic.tiles[i] });
		}

		std::function<void()> func = std::bind(&ProgressiveRender::RunPass, this);
		SharedTask initTask(Task::MakeTask<void>(func));
		threadPool.Enqueue(initTask);
	}
}

void ProgressiveRender::Stop() {
	threadPool.~ThreadPool();
}

void ProgressiveRender::Clear() {
	renderDirective.film->Clear();
}

void ProgressiveRender::RunPass() {
	tileTasks.clear();
	UpdateOutputTexture();
	std::function<void()> runPassFunc = std::bind(&ProgressiveRender::RunPass, this);
	SharedTask runTask(Task::MakeTask<void>(runPassFunc));
	
	const unsigned numTiles = renderMosaic.tiles.size();
	tileTasks.reserve(numTiles);
	for (unsigned i = 0; i < numTiles; ++i) {
		std::function<void()> tileFunc = std::bind(&TileTaskPackage::Work, &tileTaskPackages[i]);
		SharedTask tileTask(Task::MakeTask<void>(tileFunc));
		tileTasks.push_back(tileTask);
		runTask->WaitFor(*tileTask.get());
		threadPool.Enqueue(tileTask);
	}
	
	threadPool.Enqueue(runTask);
}

void ProgressiveRender::UpdateOutputTexture() {
	renderDirective.film->ToRGBTexture(&outputTexture);
	if (updateCallback) updateCallback();
}

LAMBDA_END