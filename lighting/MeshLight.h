#pragma once
#include <core/TriangleMesh.h>
#include <shading/TextureAdapter.h>
#include "Light.h"

class TriangleLight : public Light {
	public:
		TextureAdapter emission;

		TriangleLight(TriangleMesh *_mesh, Texture *_emission) {
			mesh = _mesh;
			emission.texture = _emission;
			emission.type = SpectrumType::Illuminant;
		}

		void ComputeArea() {
			const Vec3 v0(mesh->[triangle->v0].x, mesh->vertices[_tri.v0].y, mesh->vertices[_tri.v0].z);
			const Vec3 v1(mesh->vertices[_tri.v1].x, mesh->vertices[_tri.v1].y, mesh->vertices[_tri.v1].z);
			const Vec3 v2(mesh->vertices[_tri.v2].x, mesh->vertices[_tri.v2].y, mesh->vertices[_tri.v2].z);
			area = maths::Cross(v1 - v0, v2 - v0).Magnitude() * .5;
		}

		Spectrum Sample_Li() const override {

		}

	protected:		
		Real area;
		Triangle *triangle;
		TriangleMesh *mesh;
};