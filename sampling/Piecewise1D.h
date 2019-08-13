#pragma once
#include <vector>
#include <maths/maths.h>

namespace Sampling {

	class Piecewise1D {
		public:
			std::vector<Real> pdf, cdf;
			Real integral;

			Piecewise1D(const Real *_d, const unsigned _n) : pdf(_d, _d + _n), cdf(_n + 1) {
				cdf[0] = 0;
				for (unsigned i = 1; i < _n; ++i) {
					cdf[i] = cdf[i - 1] + pdf[i - 1] / _n;
				}
				integral = cdf[_n];
				const Real inv = 1. / integral;
				for (unsigned i = 1; i < _n; ++i)
					cdf[i] *= inv;
			}

			Real Sample(const Real _u, Real *_pdf, int *_off = nullptr) const {

			}
	};

}