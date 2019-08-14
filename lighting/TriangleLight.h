#pragma once
#include <core/TriangleMesh.h>
#include <shading/TextureAdapter.h>
#include "Light.h"

class TriangleLight : public Light {
	public:
		TextureAdapter emission;

		TriangleLight(Triangle *_triangle, TriangleMesh *_mesh, Texture *_emission) {
			mesh = _mesh;
			triangle = _triangle;
			emission.texture = _emission;
			emission.type = SpectrumType::Illuminant;
			ComputeArea();
		}

		void ComputeArea() {
			const Vec3 v0(mesh->vertices[triangle->v0].x, mesh->vertices[triangle->v0].y, mesh->vertices[triangle->v0].z);
			const Vec3 v1(mesh->vertices[triangle->v1].x, mesh->vertices[triangle->v1].y, mesh->vertices[triangle->v1].z);
			const Vec3 v2(mesh->vertices[triangle->v2].x, mesh->vertices[triangle->v2].y, mesh->vertices[triangle->v2].z);
			area = maths::Cross(v1 - v0, v2 - v0).Magnitude() * .5;
		}

		Spectrum Sample_Li(const Vec3 &_p, const Vec2 &_u, Real &_pdf) const override {
			const Vec3 p = SamplePoint(_u);

		}

		Real Pdf_Li(const Vec3 &_p, const Vec3 &_wi) const override {

		}

	protected:
		Real area;
		Triangle *triangle;
		TriangleMesh *mesh;

		inline Vec3 SamplePoint(const Vec2 &_u) const {
			const Vec3 v0(mesh->vertices[triangle->v0].x, mesh->vertices[triangle->v0].y, mesh->vertices[triangle->v0].z);
			const Vec3 v1(mesh->vertices[triangle->v1].x, mesh->vertices[triangle->v1].y, mesh->vertices[triangle->v1].z);
			const Vec3 v2(mesh->vertices[triangle->v2].x, mesh->vertices[triangle->v2].y, mesh->vertices[triangle->v2].z);
			return v0 + (v1 - v0) * _u.x + (v2 - v0) * _u.y;
		}
};