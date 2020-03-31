/*---- Sam Warren 2019-2020 ----
Encapsulation of visual properties of a material. Shader graph's for material should be kept here.

*/
#pragma once
#include <utility/Memory.h>
#include "media/Media.h"

LAMBDA_BEGIN

class Light;
class BxDF;
struct Socket;

class Material {
	public:
		std::string name;
		MediaBoundary mediaBoundary;
		BxDF *bxdf;
		Light *light;
		MemoryArena graphArena;
		//Socket *_albedo;

		Material();

		Socket *FindAlbedoSocket() const {

		}
 };

LAMBDA_END