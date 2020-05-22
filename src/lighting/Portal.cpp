#include "Portal.h"

LAMBDA_BEGIN

MeshPortal::MeshPortal(EnvironmentLight *_parentLight, TriangleMesh *_mesh) {
	parentLight = _parentLight;
	mesh = _mesh;
	_mesh->material->light = this;
	InitDistribution();
}

/*
	Speed up intersections when no volumes are present.
*/
static inline bool Intersect(const Ray &_ray, RayHit &_hit, const Scene &_scene, Sampler &_sampler, Medium *_med) {
	return _scene.hasVolumes ? _scene.IntersectTr(_ray, _hit, _sampler, _med) : _scene.Intersect(_ray, _hit);
}

Spectrum MeshPortal::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const unsigned i = triDistribution.SampleDiscrete(_sampler->Get1D(), &_pdf);
	const Vec2 u = _sampler->Get2D();
	const Vec3 p = mesh->SamplePoint(mesh->triangles[i], u);
	Spectrum Tr(1);
	if (MutualVisibility(_event.hit->point + _event.hit->normalG * .00001, p, _event, *_event.scene, *_sampler, &Tr)) {
		Real triArea;
		Vec3 normal;
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triArea, &normal);
		const Real denom = -maths::Dot(normal, _event.wi) * triArea;
		if (denom > 0) {
			_event.wiL = _event.ToLocal(_event.wi);
			_pdf *= maths::DistSq(_event.hit->point, p) / denom;
			return parentLight->Le(_event.wi) * Tr;
		}
	}
	_pdf = 0;
	return Spectrum(0);
}

Real MeshPortal::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	RayHit hit;
	if (!Intersect(Ray(_event.hit->point + _event.hit->normalG * .00001, _event.wi), hit, *_event.scene, _sampler, _event.medium)) return 0;
	if (hit.object->material->light != this) return 0;
	Real triArea;
	Vec3 normal;
	mesh->GetTriangleAreaAndNormal(&mesh->triangles[hit.primId], &triArea, &normal);
	const Real denom = -maths::Dot(normal, _event.wi) * triArea;
	if (denom > 0) return maths::DistSq(_event.hit->point, hit.point) / denom;
	return 0;
}

Spectrum MeshPortal::L(const ScatterEvent &_event) const {
	if (maths::Dot(_event.hit->normalS, _event.wo) > 0) {
		return parentLight->Le(-_event.wo);
	}
	return Spectrum(0);
}

Real MeshPortal::Area() const {
	return mesh->Area();
}

Real MeshPortal::Irradiance() const {
	return parentLight->intensity;
}

Bounds MeshPortal::GetBounds() const {
	return mesh->GetBounds();
}

void MeshPortal::InitDistribution() {
	const size_t ts = mesh->trianglesSize;
	std::unique_ptr<Real[]> triAreas(new Real[ts]);
	for (size_t i = 0; i < ts; ++i) {
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triAreas[i]);
	}
	triDistribution = Distribution::Piecewise1D(&triAreas[0], ts);
}

LAMBDA_END