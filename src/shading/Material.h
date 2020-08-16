/*---- Sam Warren 2019-2020 ----
Encapsulation of visual properties of a material. Shader graph's for material should be kept here.

*/
#pragma once
#include <unordered_map>
#include <utility/Memory.h>
#include "graph/ShaderGraph.h"
#include "media/Media.h"

LAMBDA_BEGIN

class Light;
class BxDF;
struct Socket;

/*
	Used to find important specific sockets in a shader graph. E.g. to find albedo socket for albedo pass.
*/
class Material {
	public:
		std::string name;
		MediaBoundary mediaBoundary;
		BxDF *bxdf;
		Light *light;
		MemoryArena graphArena;
		std::unordered_map<std::string, ShaderGraph::Socket *> socketMap;

		Material();

		void BuildSocketMap();

		ShaderGraph::Socket *GetSocket(const std::string &_key) const;
 };

LAMBDA_END