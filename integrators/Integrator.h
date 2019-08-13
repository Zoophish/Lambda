#pragma once
#include <core/Spectrum.h>
#include <core/Scene.h>

class Integrator {
	public:

		virtual Spectrum Li(const Ray &_ray, const Scene &_scene) {

		}
};