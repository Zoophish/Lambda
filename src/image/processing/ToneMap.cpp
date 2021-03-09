//ACES implmentation from: https://github.com/TheRealMJP/BakingLab/blob/master/BakingLab/ACES.hlsl - Stephen Hill
#include "ToneMap.h"


static const Vec3 ACESInputMat[3] =
{
	{0.59719, 0.35458, 0.04823},
	{0.07600, 0.90834, 0.01566},
	{0.02840, 0.13383, 0.83777}
};

static const Vec3 ACESOutputMat[3] =
{
	{ 1.60475, -0.53108, -0.07367},
	{-0.10208,  1.10813, -0.00605},
	{-0.00327, -0.07276,  1.07602}
};

static inline Vec3 MulVec3Mat3x3(const Vec3 *_mat, const Vec3 &_vec) {
	const Vec3 v0 = _mat[0] * _vec.x;
	const Vec3 v1 = _mat[1] * _vec.y;
	const Vec3 v2 = _mat[2] * _vec.z;
	return v0 + v1 + v2;
}

/*
	Reference rending transform and output device transform
*/
static Vec3 RRTAndOTDFit(Vec3 _v) {
	Vec3 a = _v * (_v + 0.0245786f) - 0.000090537f;
	Vec3 b = _v * (_v * 0.983729f + 0.4329510f) + 0.238081f;
	return a / b;
}


LAMBDA_BEGIN

static void ApplyTransform(Texture *_target, void(*_Transform)(Colour &)) {
	const size_t s = _target->GetWidth() * _target->GetHeight();
	for (size_t i = 0; i < s; ++i) _Transform((*_target)[i]);
}

static void ClampTextureChannels(Texture *_target) {
	const size_t s = _target->GetWidth() * _target->GetHeight();
	for (size_t i = 0; i < s; ++i) {
		Colour &c = (*_target)[i];
		c.r = maths::Clamp(c.r, (Real)0, (Real)1);
		c.g = maths::Clamp(c.g, (Real)0, (Real)1);
		c.b = maths::Clamp(c.b, (Real)0, (Real)1);
	}
}

static void AcesFitted(Colour &_colourRef) {
	Vec3 c(&_colourRef.r);
	c = MulVec3Mat3x3(ACESInputMat, c);
	c = RRTAndOTDFit(c);
	c = MulVec3Mat3x3(ACESOutputMat, c);
	_colourRef = Colour(c.x, c.y, c.z);
}

namespace PostProcessing {

	ToneMap::ToneMap(const ToneMapMethod _method) {
		method = _method;
	}

	void ToneMap::Process(Texture *_target) const {
		switch (method) {
		case ToneMapMethod::ACES_FILMIC:
			ApplyTransform(_target, &AcesFitted);
		}
		if (clamp) ClampTextureChannels(_target);
	}

}
LAMBDA_END