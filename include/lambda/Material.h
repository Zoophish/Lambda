#pragma once
#include "APIUtilities.h"

LAMBDA_API_NAMESPACE_BEGIN



/* Opaque Types */

struct LAMBDA_Material;
struct LAMBDA_Light;
struct LAMBDA_Shader;
struct LAMBDA_ShaderNode;



/* Creates a new material. */
LAMBDA_API LAMBDA_Material *lambdaCreateMaterial();

/* Assigns _light to _material. */
LAMBDA_API void lambdaSetMaterialLight(LAMBDA_Material *_material, LAMBDA_Light *_light);

/* Assigns the bxdf output socket in _node to _material. */
LAMBDA_API void lambdaSetMaterialBXDF(LAMBDA_Material *_material, LAMBDA_ShaderNode *_node);



/* Light types */
enum LAMBDA_LightType {
	LAMBDA_LIGHT_POINT,
	LAMBDA_LIGHT_MESH,
	LAMBDA_LIGHT_ENVIRONMENT,
	LAMBDA_LIGHT_SPOT,
};

/* Creates a light object. */
LAMBDA_API LAMBDA_Light *lambdaCreateLight(LAMBDA_LightType _type);



/* Shader Node Types */
enum LAMBDA_ShaderNodeType {
	LAMBDA_NODE_BXDF_MIX,
	LAMBDA_NODE_BXDF_LAMBERTIAN,
	LAMBDA_NODE_BXDF_OREN_NAYAR_R,
	LAMBDA_NODE_BXDF_OREN_NAYAR_T,
	LAMBDA_NODE_BXDF_MICROFACET_R,
	LAMBDA_NODE_BXDF_MICROFACET_T,
	LAMBDA_NODE_BXDF_GHOST,
	LAMBDA_NODE_BXDF_FRESNEL,
	LAMBDA_NODE_BXDF_SPECULAR_R,
	LAMBDA_NODE_BXDF_SPECULAR_T,
	LAMBDA_NODE_SEPARATE_XYZ,
	LAMBDA_NODE_SEPARATE_RGBA,
	LAMBDA_NODE_MERGE_XYZ,
	LAMBDA_NODE_MERGE_RGBA,
	LAMBDA_NODE_SCALAR,
	LAMBDA_NODE_VECTOR3,
	LAMBDA_NODE_VECTOR2,
	LAMBDA_NODE_RGB,
	LAMBDA_NODE_SPECTRUM,
	LAMBDA_NODE_BLACKBODY,
	LAMBDA_NODE_IMAGE_TEXTURE,
	LAMBDA_NODE_SURFACE_INFO,
	LAMBDA_NODE_MATHS,
	LAMBDA_NODE_VECTOR_MATHS,
	LAMBDA_NODE_DOT_PRODUCT,
	LAMBDA_NODE_CROSS_PRODUCT,
	LAMBDA_NODE_VECTOR_MAGNITUDE,
	LAMBDA_NODE_PERLIN_NOISE,
	LAMBDA_NODE_VALUE_NOISE,
	LAMBDA_NODE_OCTAVE_NOISE,
	LAMBDA_NODE_CHECKER,
	LAMBDA_NODE_VORONOI
};

/* Creates a new shader. */
LAMBDA_API LAMBDA_Shader *lambdaCreateShader();

/* Add a new node to _shader. */
LAMBDA_API LAMBDA_ShaderNode *lambdaCreateShaderNode(LAMBDA_Shader *_shader, LAMBDA_ShaderNodeType _nodeType);

/* Link an output socket of a node to another's input socket. */
LAMBDA_API void lambdaLinkSocketsTag(LAMBDA_ShaderNode *_outNode, char *_outSocketTag, LAMBDA_ShaderNode *_inNode, char *_inSocketTag);

/* Link an output socket of a node to another's input socket. */
LAMBDA_API void lambdaLinkSockets(LAMBDA_ShaderNode *_outputNode, int _outSocketIndex, LAMBDA_ShaderNode *_inputNode, int _inSocketIndex);




LAMBDA_API_NAMESPACE_END