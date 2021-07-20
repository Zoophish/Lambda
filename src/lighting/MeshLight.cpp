#include "MeshLight.h"

LAMBDA_BEGIN

MeshLight::MeshLight() {
	mesh = nullptr;
}

MeshLight::MeshLight(TriangleMesh *_mesh) {
	mesh = _mesh;
	_mesh->material->light = this;
	InitDistribution();
}

/*
	Speed up intersections when no volumes are present.
*/
static inline bool Intersect(const Ray &_ray, RayHit &_hit, const Scene &_scene, Sampler &_sampler, Medium *_med) {
	return _scene.hasVolumes ? _scene.IntersectTr(_ray, _hit, _sampler, _med, nullptr) : _scene.Intersect(_ray, _hit);
}

Spectrum MeshLight::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const unsigned i = triDistribution.SampleDiscrete(_sampler->Get1D(), &_pdf);
	const Vec2 u = _sampler->Get2D();
	const Vec3 pL = mesh->SamplePointInTriangle(mesh->triangles[i], u);
	const Vec3 pS = _event.hit->point + _event.hit->normalG * SURFACE_EPSILON * _event.sidedness;
	Spectrum Tr(1);
	if (MutualVisibility(pS, pL, _event, *_event.scene, *_sampler, &Tr)) {
		Real triArea;
		Vec3 normal;
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triArea, &normal);
		const Real cosTheta = std::abs(maths::Dot(normal, -_event.wi));
		if (cosTheta > 0) {
			_event.wiL = _event.ToLocal(_event.wi);
			_pdf *= maths::DistSq(_event.hit->point, pL) / (cosTheta * triArea);
			return emission->GetAsSpectrum(_event, SpectrumType::Illuminant) * Tr * intensity * INV_PI;
		}
	}
	_pdf = 0;
	return Spectrum(0);
}

Real MeshLight::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	RayHit hit;
	const Ray r(_event.hit->point + _event.hit->normalG * SURFACE_EPSILON * _event.sidedness, _event.wi);
	if (!Intersect(r, hit, *_event.scene, _sampler, _event.medium)) return 0;
	if (hit.object->material->light != this) return 0;
	Real triArea;
	Vec3 normal;
	mesh->GetTriangleAreaAndNormal(&mesh->triangles[hit.primId], &triArea, &normal);
	const Real cosTheta = std::abs(maths::Dot(normal, -_event.wi));
	if (cosTheta > 0) return maths::DistSq(_event.hit->point, hit.point) / (cosTheta * triArea);
	return 0;
}

Real MeshLight::PDF_Li(const ScatterEvent &_event) const {
	if (maths::Dot(_event.wi, _event.hit->normalG) > 0) return 0;	//One sided, _event info is incomplete at this stage so dot must be used
	const Real triPdf = triDistribution.PDF(_event.hit->primId);
	const Real distSq = _event.hit->tFar * _event.hit->tFar;
	Real triArea;
	mesh->GetTriangleAreaAndNormal(&mesh->triangles[_event.hit->primId], &triArea);
	const Real cosTheta = std::abs(maths::Dot(_event.hit->normalG, -_event.wi));	//abs for double sided
	return triPdf * distSq / (cosTheta * triArea);
}

Spectrum MeshLight::SamplePoint(Sampler &_sampler, ScatterEvent &_event, PartialLightSample *_ls) const {
	Real distPDF;
	const unsigned i = triDistribution.SampleDiscrete(_sampler.Get1D(), &distPDF);
	Real area;
	const Triangle &t = mesh->triangles[i];
	mesh->GetTriangleAreaAndNormal(&t, &area, &_ls->normal);
	_ls->pdf *= distPDF / area;
	const Vec2 u = _sampler.Get2D();
	_event.hit->uvCoords = maths::BarycentricInterpolation(
		mesh->textureCoordinates[t.v0],
		mesh->textureCoordinates[t.v1],
		mesh->textureCoordinates[t.v2],
		u.x, u.y);
	_ls->point = mesh->SamplePointInTriangle(mesh->triangles[i], u);	//Maybe add normal * epsilon?
	return emission->GetAsSpectrum(_event, SpectrumType::Illuminant) * intensity * INV_PI;
}

Spectrum MeshLight::Visibility(const Vec3 &_shadingPoint, ScatterEvent &_event, Sampler &_sampler, PartialLightSample *_ls) const {
	Spectrum Tr(1);
	if (MutualVisibility(_shadingPoint, _ls->point, _event, *_event.scene, _sampler, &Tr)) {
		const Real cosTheta = std::abs(maths::Dot(_ls->normal, -_event.wi));
		_ls->pdf *= maths::DistSq(_shadingPoint, _ls->point) / cosTheta;
		return Tr;
	}
	_ls->pdf = 0;
	return Spectrum(0);
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

Bounds MeshLight::GetBounds() const {
	return mesh->GetBounds();
}

Vec3 MeshLight::GetDirection() const {
	return Vec3(0, 0, 0);
}

TriangleMesh const &MeshLight::GetMesh() const {
	return *mesh;
}

void MeshLight::InitDistribution() {
	const size_t ts = mesh->numTriangles;
	std::unique_ptr<Real[]> triAreas(new Real[ts]);
	for (size_t i = 0; i < ts; ++i) {
		mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triAreas[i]);
	}
	triDistribution = Distribution::Piecewise1D(&triAreas[0], ts);
}



TriangleLight::TriangleLight(MeshLight *_meshLight, const size_t _i) {
	meshLight = _meshLight;
	triIndex = _i;
}

Spectrum TriangleLight::Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const {
	const Vec3 pL = meshLight->mesh->SamplePointInTriangle(meshLight->mesh->triangles[triIndex], _sampler->Get2D());
	const Vec3 pS = _event.hit->point + _event.hit->normalG * SURFACE_EPSILON * _event.sidedness;
	Spectrum Tr(1);
	if (MutualVisibility(pS, pL, _event, *_event.scene, *_sampler, &Tr)) {
		Real triArea;
		Vec3 normal;
		meshLight->mesh->GetTriangleAreaAndNormal(&meshLight->mesh->triangles[triIndex], &triArea, &normal);
		const Real cosTheta = std::abs(maths::Dot(normal, -_event.wi));	//Pdf to solid angle measure: wi is reversed, changing sign of dot is faster than the Vec3.
		if (cosTheta > 0) {
			_event.wiL = _event.ToLocal(_event.wi);
			_pdf = maths::DistSq(_event.hit->point, pL) * (cosTheta * triArea);
			return meshLight->emission->GetAsSpectrum(_event, SpectrumType::Illuminant) * Tr * meshLight->intensity * INV_PI;
		}
	}
	_pdf = 0;
	return Spectrum(0);
}

Real TriangleLight::PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const {
	return meshLight->PDF_Li(_event, _sampler);
}

Real TriangleLight::PDF_Li(const ScatterEvent &_event) const {
	if (maths::Dot(_event.wi, _event.hit->normalG) > 0) return 0;	//One sided, _event info is incomplete at this stage so dot must be used
	const Real distSq = _event.hit->tFar * _event.hit->tFar;
	Real triArea;
	meshLight->mesh->GetTriangleAreaAndNormal(&meshLight->mesh->triangles[_event.hit->primId], &triArea);
	const Real cosTheta = std::abs(maths::Dot(_event.hit->normalG, -_event.wi));	//abs for double sided
	return distSq / (cosTheta * triArea);
}

Spectrum TriangleLight::SamplePoint(Sampler &_sampler, ScatterEvent &_event, PartialLightSample *_ls) const {
	const TriangleMesh &mesh = *meshLight->mesh;
	const Triangle &t = mesh.triangles[triIndex];
	Real area;
	mesh.GetTriangleAreaAndNormal(&t, &area);
	_ls->pdf /= area;
	const Vec2 u = _sampler.Get2D();
	_event.hit->uvCoords = maths::BarycentricInterpolation(
		mesh.textureCoordinates[t.v0],
		mesh.textureCoordinates[t.v1],
		mesh.textureCoordinates[t.v2],
		u.x, u.y);
	_ls->point = mesh.SamplePointInTriangle(t, u);
	return meshLight->emission->GetAsSpectrum(_event, SpectrumType::Illuminant) * meshLight->intensity * INV_PI;
}

Spectrum TriangleLight::Visibility(const Vec3 &_shadingPoint, ScatterEvent &_event, Sampler &_sampler, PartialLightSample *_ls) const {
	Spectrum Tr(1);
	if (MutualVisibility(_shadingPoint, _ls->point, _event, *_event.scene, _sampler, &Tr)) {
		const Real cosTheta = std::abs(maths::Dot(_event.hit->normalG, -_event.wi));
		_ls->pdf *= maths::DistSq(_shadingPoint, _ls->point) / cosTheta;
		return Tr;
	}
	_ls->pdf = 0;
	return Spectrum(0);
}

Spectrum TriangleLight::L(const ScatterEvent &_event) const {
	return meshLight->L(_event);
}

Real TriangleLight::Area() const {
	Real area;
	meshLight->mesh->GetTriangleAreaAndNormal(&meshLight->mesh->triangles[triIndex], &area);
	return area;
}

Real TriangleLight::Irradiance() const {
	//Approximation of irradiance by sampling emission texture inside triangle.
	constexpr unsigned samples = 3;	//samples used = samples^2
	constexpr Real invSamples = (Real)1 / (Real)samples;
	ScatterEvent fakeEvent;	//TO SELF: This needs a design reconsideration in ShaderGraph
	RayHit fakeHit;
	fakeEvent.hit = &fakeHit;
	const Vec2 &uv0 = meshLight->mesh->textureCoordinates[meshLight->mesh->triangles[triIndex].v0];
	const Vec2 &uv1 = meshLight->mesh->textureCoordinates[meshLight->mesh->triangles[triIndex].v1];
	const Vec2 &uv2 = meshLight->mesh->textureCoordinates[meshLight->mesh->triangles[triIndex].v2];
	Real irradiance = 0;
	for (unsigned a = 0; a < samples; ++a) {
		for (unsigned b = 0; b < samples; ++b) {
			fakeHit.uvCoords = maths::BarycentricInterpolation(uv0, uv1, uv2, (Real)a * invSamples, (Real)b * invSamples);
			irradiance += meshLight->emission->GetAsSpectrum(fakeEvent, SpectrumType::Illuminant).y();
		}
	}
	return irradiance * invSamples * invSamples * meshLight->intensity;
}

Bounds TriangleLight::GetBounds() const {
	const Vec3 &p0 = meshLight->mesh->vertices[meshLight->mesh->triangles[triIndex].v0];
	const Vec3 &p1 = meshLight->mesh->vertices[meshLight->mesh->triangles[triIndex].v1];
	const Vec3 &p2 = meshLight->mesh->vertices[meshLight->mesh->triangles[triIndex].v2];
	Bounds bounds(p0);
	bounds = maths::Union(bounds, p1);
	bounds = maths::Union(bounds, p2);
	return bounds;
}

Vec3 TriangleLight::GetDirection() const {
	Vec3 normal;
	Real area;
	meshLight->mesh->GetTriangleAreaAndNormal(&meshLight->mesh->triangles[triIndex], &area, &normal);
	return normal;
}

LAMBDA_END