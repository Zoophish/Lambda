#pragma once
#include <vector>
#include <memory>
#include <maths/maths.h>

namespace Distribution {

	template<class T>
	inline unsigned FindInterval(const int _size, const T &_predicate) {
		int first = 0, len = _size;
		while (len > 0) {
			int half = len >> 1, middle = first + half;
			if (_predicate(middle)) {
				first = middle + 1;
				len -= half + 1;
			}
			else
				len = half;
		}
		return maths::Clamp(first - 1, 0, _size - 2);
	}
	
	class Piecewise1D {
		public:
			Piecewise1D() {}

			Piecewise1D(const Real *_d, const unsigned _n) : pdf(_d, _d + _n), cdf(_n + 1) {
				cdf[0] = 0;
				for (int i = 1; i < _n + 1; ++i) {
					cdf[i] = cdf[i - 1] + pdf[i - 1] / _n;
				}
				integral = cdf[_n];
				const Real inv = 1. / integral;
				if (integral == 0) {
					for (int i = 1; i < _n + 1; ++i)
						cdf[i] = Real(i) / Real(_n);
				}
				else {
					for (int i = 1; i < _n + 1; ++i)
						cdf[i] /= integral;
				}
			}

			Real SampleContinuous(const Real _u, Real *_pdf, int *_off = nullptr) const {
				const unsigned offset = FindInterval(cdf.size(), [&](int index) { return cdf[index] <= _u; });
				if (_off) *_off = offset;
				Real du = _u - cdf[offset];
				if ((cdf[offset + 1] - cdf[offset]) > 0) {
					du /= (cdf[offset + 1] - cdf[offset]);
				}
				if (_pdf) *_pdf = pdf[offset] / integral;
				return (offset + du) / pdf.size();
			}

		protected:
			friend class Piecewise2D;
			Real integral;
			std::vector<Real> pdf, cdf;
	};

	class Piecewise2D {
		public:
			Piecewise2D() {}

			Piecewise2D(const Real *_pdf, const unsigned _nu, const unsigned _nv) {
				for (unsigned v = 0; v < _nv; ++v) {
					pConditionalV.emplace_back(new Piecewise1D(&_pdf[v * _nu], _nu));
				}
				std::vector<Real> marginalFunc;
				for (int v = 0; v < _nv; ++v) {
					marginalFunc.push_back(pConditionalV[v]->integral);
				}
				pMarginal = Piecewise1D(&marginalFunc[0], _nv);
			}

			Vec2 SampleContinuous(const Vec2 &_u, Real *_pdf) const {
				Real pdfs[2];
				int v;
				const Real d1 = pMarginal.SampleContinuous(_u.y, &pdfs[1], &v);
				const Real d0 = pConditionalV[v]->SampleContinuous(_u.x, &pdfs[0]);
				*_pdf = pdfs[0] * pdfs[1];
				return Vec2(d0, d1);
			}

			Real PDF(const Vec2 &_uv) const {
				int iu = maths::Clamp(int(_uv.x * pConditionalV[0]->pdf.size()),
					0, (int)pConditionalV[0]->pdf.size() - 1);
				int iv = maths::Clamp(int(_uv.y * pMarginal.pdf.size()),
					0, (int)pMarginal.pdf.size() - 1);
				return pConditionalV[iv]->pdf[iu] / pMarginal.integral;
			}

		protected:
			std::vector<Piecewise1D*> pConditionalV;
			Piecewise1D pMarginal;
	};

}