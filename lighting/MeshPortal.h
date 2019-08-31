/*----	By Sam Warren 2019	----
Similar to MeshLight, however, the environment light's emission is sampled instead.
Does not importance sample more luminant areas of IBL map, rather ensures shadow rays pass through portal's area.
Bad placement of portals will worsen convergence.
It is redundant to add the portal mesh as an object to the scene as portals only need to be light sampled.
*/

#pragma once
#include <core/TriangleMesh.h>
#include <sampling/Piecewise.h>
#include "EnvironmentLight.h"

class MeshPortal : public Light {
	public:
		EnvironmentLight *parentLight;

		MeshPortal(EnvironmentLight *_parentLight, TriangleMesh *_mesh) {
			parentLight = _parentLight;
			mesh = _mesh;
			_mesh->light = this;
			parentLight->radius = 0.1; //Prevents over sampling of parent.
			InitDistribution();
		}

		Spectrum Sample_Li(SurfaceScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override {
			const unsigned i = triDistribution.SampleDiscrete(_sampler->Get1D(), &_pdf);
			const Vec2 u = _sampler->Get2D();
			const Vec3 p = mesh->SamplePoint(mesh->triangles[i], u);
			if (_event.scene->MutualVisibility(_event.hit->point + _event.hit->normalG * .001, p)) {
				_event.wi = (p - _event.hit->point).Normalised();
				Real triArea;
				Vec3 normal;
				mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triArea, &normal);
				const Real denom = maths::Dot(normal, -_event.wi) * triArea;
				if (denom > 0) {
					_event.wiL = _event.ToLocal(_event.wi);
					_pdf *= maths::DistSq(_event.hit->point, p) / denom;
					return parentLight->Le(_event.wi) * INV_PI;
				}
			}
			_pdf = 0;
			return Spectrum(0);
		}

		Real PDF_Li(const SurfaceScatterEvent &_event) const override {
			RayHit hit;
			if (!_event.scene->Intersect(Ray(_event.hit->point + _event.hit->normalG * .00001, _event.wi), hit)) return 0;
			if (hit.object->light != this) return 0;
			Real triArea;
			Vec3 normal;
			mesh->GetTriangleAreaAndNormal(&mesh->triangles[hit.primId], &triArea, &normal);
			const Real denom = maths::Dot(normal, -_event.wi) * triArea;
			if (denom > 0) return maths::DistSq(_event.hit->point, hit.point) / denom;
			return 0;
		}

		Spectrum L(const SurfaceScatterEvent &_event) const override {
			if (maths::Dot(_event.hit->normalS, _event.wo) > 0) {
				return parentLight->Le(_event.wo);
			}
			return Spectrum(0);
		}

		Real Area() const override {
			return mesh->Area();
		}

		Real Irradiance() const override {
			return parentLight->intensity;
		}

	protected:
		Distribution::Piecewise1D triDistribution;
		TriangleMesh *mesh;

		void InitDistribution() {
			const size_t ts = mesh->trianglesSize;
			std::unique_ptr<Real[]> triAreas(new Real[ts]);
			for (size_t i = 0; i < ts; ++i) {
				mesh->GetTriangleAreaAndNormal(&mesh->triangles[i], &triAreas[i]);
			}
			triDistribution = Distribution::Piecewise1D(&triAreas[0], ts);
		}
};