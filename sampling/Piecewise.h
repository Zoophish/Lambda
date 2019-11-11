#pragma once
#include <vector>
#include <memory>
/*---- Sam Warren 2019 ----
	Piecewise distibution sampling class based on PBRT:
		http://www.pbr-book.org/3ed-2018/Monte_Carlo_Integration/Sampling_Random_Variables.html
*/

#include <maths/maths.h>

namespace Distribution {
	
	class Piecewise1D {
		public:
			Piecewise1D();

			Piecewise1D(const Real *_d, const unsigned _n);

			Real SampleContinuous(const Real _u, Real *_pdf, int *_off = nullptr) const;

			unsigned SampleDiscrete(Real _u, Real *_pdf = nullptr, Real *_uRemapped = nullptr) const;

			inline Real PDF(const unsigned _i) const {
				return pdf[_i];
			}

		protected:
			friend class Piecewise2D;
			friend class FrangiblePiecewise2D;
			Real integral;
			std::vector<Real> pdf, cdf;
	};



	class Piecewise2D {
		public:
			Piecewise2D();

			Piecewise2D(const Real *_pdf, const unsigned _nu, const unsigned _nv);

			Vec2 SampleContinuous(const Vec2 &_u, Real *_pdf) const;

			Real PDF(const Vec2 &_uv) const;

		protected:
			std::vector<Piecewise1D*> pConditionalV;
			Piecewise1D pMarginal;
	};




	class FrangiblePiecewise2D {
		public:
			FrangiblePiecewise2D();

			FrangiblePiecewise2D(const Real *_pdf, const unsigned _nu, const unsigned _nv);

			Vec2 SampleContinuous(const Vec2 &_u, Real *_pdf) const;

			Vec2 SampleContinuous(const Vec2 &_u, Real *_pdf, const Real _x0, const Real _x1, const Real _y0, const Real _y1) const;

			Real PDF(const Vec2 &_uv) const;

		private:
			unsigned nu, nv;
			std::vector<Piecewise1D*> pConditionalV;
			Piecewise1D pMarginal;
			std::vector<Real> summedAreaTable;

			inline Real I(const Real _x0, const Real _x1, const Real _y0, const Real _y1) const;
	};
}