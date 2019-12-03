#include "Piecewise.h"

namespace Distribution {

	Piecewise1D::Piecewise1D() {}

	Piecewise1D::Piecewise1D(const Real *_d, const unsigned _n) : pdf(_d, _d + _n), cdf(_n + 1) {
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

	Real Piecewise1D::SampleContinuous(const Real _u, Real *_pdf, int *_off) const {
		const unsigned offset = maths::FindInterval(cdf.size(), [&](int index) { return cdf[index] <= _u; });
		if (_off) *_off = offset;
		Real du = _u - cdf[offset];
		if ((cdf[offset + 1] - cdf[offset]) > 0) {
			du /= (cdf[offset + 1] - cdf[offset]);
		}
		if (_pdf) *_pdf = pdf[offset] / integral;
		return (offset + du) / pdf.size();
	}


	unsigned Piecewise1D::SampleDiscrete(Real _u, Real *_pdf, Real *_uRemapped) const {
		unsigned offset = maths::FindInterval(cdf.size(), [&](int index) { return cdf[index] <= _u; });
		if (_pdf) *_pdf = pdf[offset] / (integral * pdf.size());
		if (_uRemapped)
			*_uRemapped = (_u - cdf[offset]) / (cdf[offset + 1] - cdf[offset]);
		return offset;
	}



	Piecewise2D::Piecewise2D() {}

	Piecewise2D::Piecewise2D(const Real *_pdf, const unsigned _nu, const unsigned _nv) {
		for (unsigned v = 0; v < _nv; ++v) {
			pConditionalV.emplace_back(new Piecewise1D(&_pdf[v * _nu], _nu));
		}
		std::vector<Real> marginalFunc;
		for (int v = 0; v < _nv; ++v) {
			marginalFunc.push_back(pConditionalV[v]->integral);
		}
		pMarginal = Piecewise1D(&marginalFunc[0], _nv);
	}

	Vec2 Piecewise2D::SampleContinuous(const Vec2 &_u, Real *_pdf) const {
		Real pdfs[2];
		int v;
		const Real d1 = pMarginal.SampleContinuous(_u.y, &pdfs[1], &v);
		const Real d0 = pConditionalV[v]->SampleContinuous(_u.x, &pdfs[0]);
		*_pdf = pdfs[0] * pdfs[1];
		return Vec2(d0, d1);
	}

	Real Piecewise2D::PDF(const Vec2 &_uv) const {
		int iu = maths::Clamp(int(_uv.x * pConditionalV[0]->pdf.size()),
			0, (int)pConditionalV[0]->pdf.size() - 1);
		int iv = maths::Clamp(int(_uv.y * pMarginal.pdf.size()),
			0, (int)pMarginal.pdf.size() - 1);
		return pConditionalV[iv]->pdf[iu] / pMarginal.integral;
	}



	FrangiblePiecewise2D::FrangiblePiecewise2D() {}

	FrangiblePiecewise2D::FrangiblePiecewise2D(const Real *_pdf, const unsigned _nu, const unsigned _nv) {
		//Pdf may need normalizing...
		nu = _nu; nv = _nv;
		summedAreaTable.resize(_nu * _nv);
		for (unsigned y = 0; y < _nv; ++y) {
			for (unsigned x = 0; x < _nu; ++x) {
				summedAreaTable[y * _nv + x] = _pdf[y * _nv + x];
				if (y > 0) summedAreaTable[y * _nv + x] += _pdf[(y - 1) * _nv + x];
				if (x > 0) summedAreaTable[y * _nv + x] += _pdf[y * _nv + x - 1];
			}
		}
		for (unsigned v = 0; v < _nv; ++v) {
			pConditionalV.emplace_back(new Piecewise1D(&_pdf[v * _nu], _nu));
		}
		std::vector<Real> marginalFunc;
		for (int v = 0; v < _nv; ++v) {
			marginalFunc.push_back(pConditionalV[v]->integral);
		}
		pMarginal = Piecewise1D(&marginalFunc[0], _nv);
	}

	Vec2 FrangiblePiecewise2D::SampleContinuous(const Vec2 &_u, Real *_pdf) const {
		Real pdfs[2];
		int v;
		const Real d1 = pMarginal.SampleContinuous(_u.y, &pdfs[1], &v);
		const Real d0 = pConditionalV[v]->SampleContinuous(_u.x, &pdfs[0]);
		*_pdf = pdfs[0] * pdfs[1];
		return Vec2(d0, d1);
	}

	Vec2 FrangiblePiecewise2D::SampleContinuous(const Vec2 &_u, Real *_pdf, const Real _x0, const Real _x1, const Real _y0, const Real _y1) const {
		Real pdfs[2];
		int v;
		const Real u1 = _u.y / ((Real)_x1 - _x0) + _x0;
		const Real u0 = _u.x / ((Real)_y1 - _y0) + _y0;
		const Real d1 = pMarginal.SampleContinuous(_u.y, &pdfs[1], &v);
		const Real d0 = pConditionalV[v]->SampleContinuous(_u.x, &pdfs[0]);
		*_pdf = pdfs[0] * pdfs[1];

		return Vec2(d0, d1);
	}

	Real FrangiblePiecewise2D::PDF(const Vec2 &_uv) const {
		int iu = maths::Clamp(int(_uv.x * pConditionalV[0]->pdf.size()),
			0, (int)pConditionalV[0]->pdf.size() - 1);
		int iv = maths::Clamp(int(_uv.y * pMarginal.pdf.size()),
			0, (int)pMarginal.pdf.size() - 1);
		return pConditionalV[iv]->pdf[iu] / pMarginal.integral;
	}

	inline Real FrangiblePiecewise2D::I(const Real _x0, const Real _x1, const Real _y0, const Real _y1) const {
		const unsigned xMin = _x0 * (Real)nu;
		const unsigned xMax = _x1 * (Real)nu;
		const unsigned yMin = _y0 * (Real)nv;
		const unsigned yMax = _y1 * (Real)nv;
		return summedAreaTable[yMin * nu + xMin] + summedAreaTable[yMax * nu + xMax]
			- summedAreaTable[yMax * nu + xMin] - summedAreaTable[yMax * nu + xMax];
	}
}