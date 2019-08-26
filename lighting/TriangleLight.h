#pragma once
#include <shading/TextureAdapter.h>
#include <core/TriangleMesh.h>
#include <shading/SurfaceScatterEvent.h>
#include <core/Scene.h>
#include "Light.h"

class TriangleLight : public Light {
	public:
		//HDR texture.
		TextureAdapter emission;
		Real intensity = 1;

		TriangleLight(Triangle *_triangle, TriangleMesh *_mesh, Texture *_emission) {
			mesh = _mesh;
			triangle = _triangle;
			emission.SetTexture(_emission);
			emission.type = SpectrumType::Illuminant;
			ComputeInfo();
		}

		inline void ComputeInfo() {
			mesh->GetTriangleAreaAndNormal(triangle, &area, &normal);
		}

		Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override {
			const Vec2 u = _sampler->Get2D();
			const Vec3 p = SamplePoint(u);
			if (_event.scene->MutualVisibility(_event.hit->point, p)) {
				_event.wi = (p - _event.hit->point).Normalised();
				_pdf = maths::Dot(_event.wi, _event.wi) / (std::abs(maths::Dot(normal, -_event.hit->point)) * area);
				return emission.GetUV(SampleUV(u)) * intensity * INV_PI;
			}
			_pdf = 0;
			return Spectrum(0);
		}

		Real PDF_Li(const SurfaceScatterEvent &_event) const override {
			return maths::Dot(_event.wi, _event.wi) / (std::abs(maths::Dot(normal, -_event.hit->point)) * area);
		}

		Spectrum L(const SurfaceScatterEvent &_event) const override {
			return emission.GetUV(_event.hit->uvCoords) * intensity * INV_PI;
		}

		Spectrum Le(const Ray &_r) const override {
			return Spectrum(0);
		}

	protected:
		Real area;
		Vec3 normal;
		Triangle *triangle;
		TriangleMesh *mesh;

		inline Vec3 SamplePoint(const Vec2 &_u) const {
			const Vec3 v0 = mesh->vertices[triangle->v0];
			const Vec3 v1 = mesh->vertices[triangle->v1];
			const Vec3 v2 = mesh->vertices[triangle->v2];
			return v0 + (v1 - v0) * _u.x + (v2 - v0) * _u.y;
		}

		inline Vec2 SampleUV(const Vec2 &_u) const {
			const Vec2 c0 = mesh->uvs[triangle->v0];
			const Vec2 c1 = mesh->uvs[triangle->v1];
			const Vec2 c2 = mesh->uvs[triangle->v2];
			return c0 + (c1 - c0) * _u.x + (c2 - c0) * _u.y;
		}
};