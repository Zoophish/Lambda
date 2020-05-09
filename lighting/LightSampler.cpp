#pragma once
#include <core/Scene.h>
#include "LightSampler.h"

LAMBDA_BEGIN

LightSampler::LightSampler(const Scene *_scene) : scene(_scene) {}

PowerLightSampler::PowerLightSampler() {}

PowerLightSampler::PowerLightSampler(const Scene &_scene) : LightSampler(&_scene) {}

Light *PowerLightSampler::Sample(const ScatterEvent &_event, Sampler &_sampler, Real *_pdf) const {
	const unsigned i = lightDistribution.SampleDiscrete(_sampler.Get1D(), _pdf);
	return scene->lights[i];
}

void PowerLightSampler::Commit() {
	if (scene) {
		if (scene->envLight) {
			const Bounds sceneBounds = scene->GetBounds();
			const Vec3 diff = sceneBounds.max - sceneBounds.min;
			scene->envLight->radius = std::max(std::max(diff.x, diff.y), diff.z) * .5;
		}
		const unsigned size = scene->lights.size();
		std::unique_ptr<Real[]> importances(new Real[size]);
		for (unsigned i = 0; i < size; ++i) {
			importances[i] = scene->lights[i]->Power();
		}
		lightDistribution = Distribution::Piecewise1D(&importances[0], size);
	}
	else std::cout << std::endl << "WARNING: No scene given to light sampler.";
}

LAMBDA_END