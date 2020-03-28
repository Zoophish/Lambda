#include "MeshLight.h"

LAMBDA_BEGIN

MeshLight::MeshLight(TriangleMesh *_mesh) {
	mesh = _mesh;
	_mesh->light = this;
	InitDistribution();
}

/*
	Speed up intersections when no volumes are present.
*/
static inline bool Intersect(const Ray &_ray, RayHit &_hit, const Scene &_scene, Sampler &_sampler, Medium *_med) {
	return _scene.hasVolumes ? _scene.IntersectTr(_ray, _hit, _sampler, _med) : _scene.Intersect(_ray, _hit);
}

Spectrum MeshLight::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const unsigned i = triDistribution.SampleDiscrete(_sampler->Get1D(), &_pdf);
	const Vec2 u = _sampler->Get2D();
	const Vec3 pL = mesh->SamplePoint(mesh->triangles[i], u);
	const Vec3 pS = _event.hit->point + _event.hit->normalG * 1e-5;
	Spectrum Tr(1);
	if (MutualVisibility(pS, pL, _event, *_event.scene, *_sampler, &Tr)) {
		Real triArea;
		Vec3 normal;
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triArea, &normal);
		const Real denom = -maths::Dot(normal, _event.wi) * triArea;	//Pdf to solid angle measure: wi is reversed, changing sign of dot is faster than the Vec3.
		if (denom > 0) {
			_event.wiL = _event.ToLocal(_event.wi);
			_pdf *= maths::DistSq(_event.hit->point, pL) / denom;
			return emission->GetAsSpectrum(_event, SpectrumType::Illuminant) * intensity * INV_PI;
		}
	}
	_pdf = 0;
	return Spectrum(0);
}

Real MeshLight::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	RayHit hit;
	const Ray r(_event.hit->point + _event.hit->normalG * 1e-5, _event.wi);
	if (!Intersect(r, hit, *_event.scene, _sampler, _event.medium)) return 0;
	if (hit.object->light != this) return 0;
	Real triArea;
	Vec3 normal;
	mesh->GetTriangleAreaAndNormal(&mesh->triangles[hit.primId], &triArea, &normal);
	const Real denom = -maths::Dot(normal, _event.wi) * triArea;	//Pdf to solid angle measure: wi is reversed, changing sign of dot is faster than the Vec3.
	if (denom > 0) return maths::DistSq(_event.hit->point, hit.point) / denom;
	return 0;
}

Spectrum MeshLight::L(const ScatterEvent &_event) const {
	return emission->GetAsSpectrum(_event, SpectrumType::Illuminant) * intensity * INV_PI;
}

Real MeshLight::Area() const {
	return mesh->Area();
}

Real MeshLight::Irradiance() const {
	return intensity;
}

void MeshLight::InitDistribution() {
	const size_t ts = mesh->trianglesSize;
	std::unique_ptr<Real[]> triAreas(new Real[ts]);
	for (size_t i = 0; i < ts; ++i) {
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triAreas[i]);
	}
	triDistribution = Distribution::Piecewise1D(&triAreas[0], ts);
}

LAMBDA_END