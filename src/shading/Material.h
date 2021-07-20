/*---- Sam Warren 2019-2020 ----
Encapsulation of visual properties of a material. Shader graph's for material should be kept here.

*/
#pragma once
#include <unordered_map>
#include <utility/Memory.h>
#include "graph/ShaderGraph.h"
#include "graph/GraphAOV.h"
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
		//std::unordered_map<std::string, ShaderGraph::AOVOutput*> aovMap;

		Material();

		//void BuildAOVMap();

		//ShaderGraph::AOVOutput *GetAOV(const std::string &_key) const;
 };

LAMBDA_END