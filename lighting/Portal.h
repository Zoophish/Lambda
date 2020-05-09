//----	Sam Warren 2019	----
#pragma once
#include <core/TriangleMesh.h>
#include <sampling/Piecewise.h>
#include "EnvironmentLight.h"

LAMBDA_BEGIN

/*
	Similar to MeshLight, however, the environment light's emission is sampled instead.
	Does not importance sample more luminant areas of IBL map, rather ensures shadow rays pass through portal's area.
	Bad placement of portals will worsen convergence.
	It is redundant to add the portal mesh as an object to the scene as portals only need to be light sampled.
*/
class MeshPortal : public Light {
	public:
		EnvironmentLight *parentLight;

		MeshPortal(EnvironmentLight *_parentLight, TriangleMesh *_mesh);

		Spectrum Sample_Li(ScatterEvent &_event, Sampler *_sampler, Real &_pdf) const override;

		Real PDF_Li(const ScatterEvent &_event, Sampler &_sampler) const override;

		Spectrum L(const ScatterEvent &_event) const override;

		Real Area() const override;

		Real Irradiance() const override;

		Bounds GetBounds() const override;

	protected:
		Distribution::Piecewise1D triDistribution;
		TriangleMesh *mesh;

		void InitDistribution();
};

/*
	A rectagonal portal that uses a summed area table to importance sample the projected region of the portal
	on the environment map by luminance - much more effective than a mesh portal.
*/
class RectPortal : public Light {
	public:
		EnvironmentLight *parentLight;
};

LAMBDA_END