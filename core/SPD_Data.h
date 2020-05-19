/*
----	By Sam Warren 2019	----
----	For the PBRT-style Spectrum implementation.	----
----	Contains lookup tables for X, Y and Z SPD curves from the Commission Internationale l'Eclairage.	----
----	Contains lookup tables for reflectant and illuminant RGB to Spectrum curves.	----

	On initialisation, all these curve data are converted to their respective SampledSpectrum objects.
*/

#pragma once
#include <Lambda.h>
#include <maths/maths.h>

LAMBDA_BEGIN

namespace CIEData {

	// 471 samples with respect to lambda for each curve.
	// Dataset's wavelengths range from 360 to 830 nano-metres.
	static const unsigned nCIESamples = 471;
	static const Real CIE_Y_integral = 106.856895;

	extern const Real CIE_lambda[nCIESamples];
	extern const Real CIE_X[nCIESamples];
	extern const Real CIE_Y[nCIESamples];
	extern const Real CIE_Z[nCIESamples];
}

namespace BergeData {
	static const unsigned nRGB2SpectSamples = 32;

	extern const Real RGB2SpectLambda[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectWhite[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectCyan[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectMagenta[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectYellow[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectRed[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectGreen[nRGB2SpectSamples];
	extern const Real RGBRefl2SpectBlue[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectWhite[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectCyan[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectMagenta[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectYellow[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectRed[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectGreen[nRGB2SpectSamples];
	extern const Real RGBIllum2SpectBlue[nRGB2SpectSamples];
}

namespace MaterialData {
	static const unsigned nCopperSamples = 56;

	extern const Real CopperWavelengths[nCopperSamples];
	extern const Real CopperN[nCopperSamples];
	extern const Real CopperK[nCopperSamples];
}

LAMBDA_END