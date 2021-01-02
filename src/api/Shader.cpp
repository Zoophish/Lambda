#pragma once
#include <lambda/Shader.h>

#include <utility/Memory.h>
#include <shading/graph/GraphBxDF.h>
#include <shading/graph/GraphConverters.h>
#include <shading/graph/GraphInputs.h>
#include <shading/graph/GraphMaths.h>
#include <shading/graph/GraphTexture.h>

namespace sg = lambda::ShaderGraph;

LAMBDA_API_NAMESPACE_BEGIN

struct LAMBDAShader {
	MemoryArena shaderArena;
};

struct LAMBDAShaderNode {
	lambda::ShaderGraph::Node *node;
	LAMBDAShaderNodeType nodeType;
};

LAMBDAShader *lambdaCreateShader() {
	LAMBDAShader *shader = new LAMBDAShader;
	return shader;
}

LAMBDAShaderNode *lambdaCreateShaderNode(LAMBDAShader *_shader, LAMBDAShaderNodeType _nodeType) {

	LAMBDAShaderNode *node = new LAMBDAShaderNode();
	MemoryArena &arena = _shader->shaderArena;

	switch (_nodeType) {
	case LAMBDA_NODE_BXDF_MIX:
		node->node = arena.New<sg::MixBxDFNode>();
		break;
	case LAMBDA_NODE_BXDF_LAMBERTIAN:
		node->node = arena.New<sg::LambertianBRDFNode>();
		break;
	case LAMBDA_NODE_BXDF_OREN_NAYAR_R:
		node->node = arena.New<sg::OrenNayarBRDFNode>();
		break;
	case LAMBDA_NODE_BXDF_OREN_NAYAR_T:
		node->node = arena.New<sg::OrenNayarBTDFNode>();
		break;
	case LAMBDA_NODE_BXDF_MICROFACET_R:
		node->node = arena.New<sg::MicrofacetBRDFNode>();
		break;
	//case LAMBDA_NODE_BXDF_MICROFACET_T:
	case LAMBDA_NODE_BXDF_GHOST:
		node->node = arena.New<sg::GhostBTDFNode>();
		break;
	case LAMBDA_NODE_BXDF_FRESNEL:
		node->node = arena.New<sg::FresnelBSDFNode>();
		break;
	case LAMBDA_NODE_BXDF_SPECULAR_R:
		node->node = arena.New<sg::SpecularBRDFNode>();
		break;
	case LAMBDA_NODE_BXDF_SPECULAR_T:
		node->node = arena.New<sg::SpecularBTDFNode>();
		break;
	case LAMBDA_NODE_SEPARATE_XYZ:
		node->node = arena.New<sg::Converter::SeparateXYZ>();
		break;
	case LAMBDA_NODE_SEPARATE_RGBA:
		node->node = arena.New<sg::Converter::SeparateRGBA>();
		break;
	case LAMBDA_NODE_MERGE_XYZ:
		node->node = arena.New<sg::Converter::MergeXYZ>();
		break;
	case LAMBDA_NODE_MERGE_RGBA:
		node->node = arena.New<sg::Converter::MergeRGBA>();
		break;
	case LAMBDA_NODE_SCALAR:
		node->node = arena.New<sg::ScalarInput>();
		break;
	//case LAMBDA_NODE_VECTOR3:
	case LAMBDA_NODE_VECTOR2:
		node->node = arena.New<sg::Vec2Input>();
		break;
	case LAMBDA_NODE_RGB:
		node->node = arena.New<sg::RGBInput>();
		break;
	case LAMBDA_NODE_SPECTRUM:
		node->node = arena.New<sg::SpectralInput>();
		break;
	case LAMBDA_NODE_BLACKBODY:
		node->node = arena.New<sg::BlackbodyInput>();
		break;
	case LAMBDA_NODE_IMAGE_TEXTURE:
		node->node = arena.New<sg::ImageTextureInput>();
		break;
	case LAMBDA_NODE_SURFACE_INFO:
		node->node = arena.New<sg::SurfaceInfoInput>();
		break;
	case LAMBDA_NODE_MATHS:
		node->node = arena.New<sg::Maths::ScalarMathsNode>();
		break;
	case LAMBDA_NODE_VECTOR_MATHS:
		node->node = arena.New<sg::Maths::VectorMathsNode>();
		break;
	case LAMBDA_NODE_DOT_PRODUCT:
		node->node = arena.New<sg::Maths::DotProductNode>();
		break;
	case LAMBDA_NODE_CROSS_PRODUCT:
		node->node = arena.New<sg::Maths::CrossProductNode>();
		break;
	case LAMBDA_NODE_VECTOR_MAGNITUDE:
		node->node = arena.New<sg::Maths::VectorLength>();
		break;
	case LAMBDA_NODE_PERLIN_NOISE:
		node->node = arena.New<sg::Textures::PerlinNoise>();
		break;
	case LAMBDA_NODE_VALUE_NOISE:
		node->node = arena.New<sg::Textures::ValueNoise>();
		break;
	case LAMBDA_NODE_OCTAVE_NOISE:
		node->node = arena.New<sg::Textures::OctaveNoise>();
		break;
	case LAMBDA_NODE_CHECKER:
		node->node = arena.New<sg::Textures::Checker>();
		break;
	case LAMBDA_NODE_VORONOI:
		node->node = arena.New<sg::Textures::Voronoi>();
	}

	node->nodeType = _nodeType;
	return node;
}

void lambdaLinkSocketsTag(LAMBDAShaderNode *_outNode, char *_outSocketTag, LAMBDAShaderNode *_inNode, char *_inSocketTag) {
	sg::Socket *outSocket = nullptr;
	outSocket = _outNode->node->GetOutputSocket(_outSocketTag);
	sg::SocketRef *inSocket = nullptr;
	inSocket = _inNode->node->GetInputSocket(_inSocketTag);
	sg::Connect(*outSocket, *inSocket);
}

void lambdaLinkSockets(LAMBDAShaderNode *_outNode, int _outSocketIndex, LAMBDAShaderNode *_inNode, int _inSocketIndex) {
	sg::Socket outSocket = _outNode->node->outputSockets[_outSocketIndex];
	sg::SocketRef &inSocket = _inNode->node->inputSockets[_inSocketIndex];
	sg::Connect(outSocket, inSocket);
}



LAMBDA_API_NAMESPACE_END