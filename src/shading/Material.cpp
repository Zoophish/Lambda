#pragma once
#include <iostream>
#include "Material.h"
#include "surface/BxDF.h"
#include "graph/ShaderGraph.h"

LAMBDA_BEGIN

//void Material::BuildAOVMap() {
//	if (ShaderGraph::Node *bxdfNode = dynamic_cast<ShaderGraph::Node *>(bxdf)) {
//		for (auto &node : *bxdfNode) {
//			if (node.nodeTag == "AOV Output") {
//				ShaderGraph::AOVOutput *aov = (ShaderGraph::AOVOutput *)&node;
//				aovMap[aov->name] = aov;
//			}
//		}
//	}
//	else std::cout << std::endl << "Could not build socket map as BxDF is not a Node.";
//}

//ShaderGraph::AOVOutput *Material::GetAOV(const std::string &_key) const {
//	if (ShaderGraph::AOVOutput *aov = aovMap.at(_key)) return aov;
//	else return nullptr;
//}

Material::Material() {
	bxdf = nullptr;
	light = nullptr;
	//aovMap.reserve(4);
}

LAMBDA_END