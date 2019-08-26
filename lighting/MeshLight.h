#pragma once
#include <sampling/Piecewise.h>
#include <core/TriangleMesh.h>
#include <shading/TextureAdapter.h>
#include <shading/SurfaceScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

class MeshLight : public Light {
	public:
		TextureAdapter emission;
		Real intensity = 1;

		MeshLight(TriangleMesh *_mesh, Texture *_emission) {
			mesh = _mesh;
			_mesh->light = this;
			emission.SetTexture(_emission);
			emission.type = SpectrumType::Illuminant;
			InitDistribution();
		}

		Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override {
			const unsigned i = triDistribution.SampleDiscrete(_sampler->Get1D(), &_pdf);
			const Vec2 u = _sampler->Get2D();
			const Vec3 p = SamplePoint(mesh->triangles[i], u);
			if (_event.scene->MutualVisibility(_event.hit->point, p)) {
				_event.wi = (p - _event.hit->point).Normalised();
				Real triArea;
				Vec3 normal;
				mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triArea, &normal);
				_pdf *= maths::DistSq(_event.hit->point, p) / (maths::Dot(normal, -_event.wi) * triArea);
				return emission.GetUV(u) * intensity * INV_PI;
			}
			_pdf = 0;
			return Spectrum(0);
		}

		Real PDF_Li(const SurfaceScatterEvent &_event) const override {
			RayHit hit;
			if (!_event.scene->Intersect(Ray(_event.hit->point, _event.wi), hit)) return 0;
			if (hit.object->light != this) return 0;
			Real triArea;
			Vec3 normal;
			mesh->GetTriangleAreaAndNormal(&mesh->triangles[hit.primId], &triArea, &normal);
			return maths::DistSq(_event.hit->point, hit.point) / (maths::Dot(normal, -_event.wi) * triArea);
		}

		Spectrum L(const SurfaceScatterEvent &_event) const override {
			return emission.GetUV(_event.hit->uvCoords) * intensity * INV_PI;
		}

		Real Area() const override {
			return mesh->Area();
		}

		Real Irradiance() const override {
			return intensity;
		}

	protected:
		TriangleMesh *mesh;
		Distribution::Piecewise1D triDistribution;

		void InitDistribution() {
			const size_t ts = mesh->trianglesSize;
			std::unique_ptr<Real[]> triAreas(new Real[ts]);
			for (size_t i = 0; i < ts; ++i) {
				mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triAreas[i]);
			}
			triDistribution = Distribution::Piecewise1D(&triAreas[0], ts);
		}

		inline Vec3 SamplePoint(const Triangle &_triangle, const Vec2 &_u) const {
			const Vec3 v0 = mesh->vertices[_triangle.v0];
			const Vec3 v1 = mesh->vertices[_triangle.v1];
			const Vec3 v2 = mesh->vertices[_triangle.v2];
			return v0 + (v1 - v0) * _u.x + (v2 - v0) * _u.y;
		}
};