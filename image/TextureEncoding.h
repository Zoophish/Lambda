/*	---- Sam Warren 2019 ----
NOTE Morton and Hilbert will only work on textures with side lengths of 2^N.
Textures may be arranged differently in memory for various benefits.
	-	Scanline stores texels row by row and is required to save a texture to a file. (i.e. Use for render textures)
	-	Morton and Hilbert ordering can improve locality preservation between 2D->1D mapping. (i.e. Use for large material textures)
	-	Hilbert and Morton slightly outperform each other in different scenarios, however Morton is more practical.

Newer processors have Binary Manipulation Instruction sets (BMI) that can be used to hardware accellerate Morton encoding/decoding.
*/
#pragma once
#define USE_BMI 1

namespace TextureEncoding {

	/*
		Texture sides MUST be of 2^N.
	*/
	size_t MortonOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y);

	/*
		Texture size MUST be of 2^N and square.
	*/
	size_t HilbertOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y);

	/*
		Any texture size works.
	*/
	size_t ScanlineRowOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y);

	/*
		Any texture size works.
	*/
	size_t ScanlineColOrder(const unsigned _w, const unsigned _h, const unsigned _x, const unsigned _y);

}
