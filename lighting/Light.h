#pragma once
#include <core/Ray.h>
#include <core/Spectrum.h>
#include <shading/SurfaceScatterEvent.h>

class Light {
	public:
		virtual Spectrum Sample_Li(const Vec3 &_p, const Vec2 &_u, Real &_pdf) const;
		
		virtual Real Pdf_Li(const Vec3 &_p, const Vec3 &_wi) const;

		virtual Spectrum Le() const;
};