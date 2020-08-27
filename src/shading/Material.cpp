#pragma once
#include "surface/BxDF.h"
#include "graph/ShaderGraph.h"
#include "Material.h"

LAMBDA_BEGIN

void Material::BuildSocketMap() {
	if (ShaderGraph::Node *bxdfNode = dynamic_cast<ShaderGraph::Node *>(bxdf)) {
		for (unsigned i = 0; i < bxdfNode->numIn; ++i) {
			if (bxdfNode->inputSockets[i].socketType == ShaderGraph::SocketType::TYPE_COLOUR) {
				socketMap["albedo"] = bxdfNode->inputSockets[i].socket;
				return;
			}
		}
	}
	else std::cout << std::endl << "Could not build catalogue as BxDF is not a Node.";
}

ShaderGraph::Socket *Material::GetSocket(const std::string &_key) const {
	if (ShaderGraph::Socket *socket = socketMap.at(_key)) return socket;
	else return nullptr;
}

Material::Material() {
	bxdf = nullptr;
	light = nullptr;
	socketMap.reserve(4);
}

LAMBDA_END