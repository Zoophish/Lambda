#pragma once
#include <lighting/EnvironmentLight.h>
#include "UtilityIntegrators.h"

LAMBDA_BEGIN

Spectrum UtilityIntegrator::Li(Ray _ray, const Scene &_scene) const {
	const Colour c = P(_ray, _scene);
	const Real cr[3] = { c.r, c.g, c.b };
	return Spectrum::FromRGB(cr, SpectrumType::Reflectance);
}



DepthPass::DepthPass(Sampler *_sampler, const Real _maxDepth, const Real _power, const Colour _background) {
	sampler = _sampler;
	power = _power;
	maxDepth = _maxDepth;
	background = _background;
}

Integrator *DepthPass::clone() const {
	return new DepthPass(*this);
}

Colour DepthPass::P(const Ray &_ray, const Scene &_scene) const {
	RayHit hit;
	if (_scene.Intersect(_ray, hit)) {
		const Real depth = std::pow((hit.point - _ray.o).Magnitude() / maxDepth, power);
		return Colour(depth, depth, depth);
	}
	return background;
}



Integrator *NormalPass::clone() const {
	return new NormalPass(*this);
}

NormalPass::NormalPass(Sampler *_sampler, const bool _normalG) {
	sampler = _sampler;
	normalG = _normalG;
}

Colour NormalPass::P(const Ray &_ray, const Scene &_scene) const {
	RayHit hit;
	if (_scene.Intersect(_ray, hit)) {
		const Vec3 n = normalG ? hit.normalG : hit.normalS;
		return Colour(n.x * (Real).5 + (Real).5, n.y * (Real).5 + 1, n.z * (Real).5 + (Real).5);
	}
	return Colour(0, 0, 0);
}



Integrator *AOVPass::clone() const {
	return new AOVPass(*this);
}

AOVPass::AOVPass(Sampler *_sampler, const std::string &_target) {
	sampler = _sampler;
	target = _target;
}

Colour AOVPass::P(const Ray &_ray, const Scene &_scene) const {
	RayHit hit;
	Colour c = { 0,0,0,1 };
	if (_scene.Intersect(_ray, hit)) {
		ScatterEvent event;
		event.hit = &hit;
		//if (ShaderGraph::AOVOutput * aov = hit.object->material->GetAOV(target)) {
		//	Spectrum s = aov->inputSockets[0].GetAsSpectrum(event);
		//	s.ToRGB((Real*)&c);
		//}
	}
	return c;
}


LAMBDA_END