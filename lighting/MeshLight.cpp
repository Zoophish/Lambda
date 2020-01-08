#include "MeshLight.h"

MeshLight::MeshLight(TriangleMesh *_mesh) {
	mesh = _mesh;
	_mesh->light = this;
	InitDistribution();
}

Spectrum MeshLight::Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const unsigned i = triDistribution.SampleDiscrete(_sampler->Get1D(), &_pdf);
	const Vec2 u = _sampler->Get2D();
	const Vec3 p = mesh->SamplePoint(mesh->triangles[i], u);
	if (_event.scene->MutualVisibility(_event.hit->point + _event.hit->normalG * 1e-5, p, &_event.wi)) {
		Real triArea;
		Vec3 normal;
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triArea, &normal);
		const Real denom = -maths::Dot(normal, _event.wi) * triArea;	//Pdf to solid angle measure: wi is reversed, changing sign of dot is faster than the Vec3.
		if (denom > 0) {
			_event.wiL = _event.ToLocal(_event.wi);
			_pdf *= maths::DistSq(_event.hit->point, p) / denom;
			return emission->GetAsSpectrum(&_event, SpectrumType::Illuminant) * intensity * INV_PI;
		}
	}
	_pdf = 0;
	return Spectrum(0);
}

Real MeshLight::PDF_Li(const SurfaceScatterEvent &_event) const {
	RayHit hit;
	if (!_event.scene->Intersect(Ray(_event.hit->point + _event.hit->normalG * 1e-5, _event.wi), hit)) return 0;
	if (hit.object->light != this) return 0;
	Real triArea;
	Vec3 normal;
	mesh->GetTriangleAreaAndNormal(&mesh->triangles[hit.primId], &triArea, &normal);
	const Real denom = -maths::Dot(normal, _event.wi) * triArea;	//Pdf to solid angle measure: wi is reversed, changing sign of dot is faster than the Vec3.
	if (denom > 0) return maths::DistSq(_event.hit->point, hit.point) / denom;
	return 0;
}

Spectrum MeshLight::L(const SurfaceScatterEvent &_event) const {
	return emission->GetAsSpectrum(&_event, SpectrumType::Illuminant) * intensity * INV_PI;
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