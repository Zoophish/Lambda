#pragma once
#include "APIUtilities.h"

LAMBDA_API_NAMESPACE_BEGIN

/* Lambda API types */

enum LAMBDAType {
	LAMBDA_DEVICE,
	LAMBDA_SCENE,
	LAMBDA_OBJECT,
	LAMBDA_TRIANGLE_MESH,
	LAMBDA_TEXTURE,
	LAMBDA_LIGHT
};



/* Opaque types */

struct LAMBDADevice;
struct LAMBDAScene;
struct LAMBDATriangleMesh;
struct LAMBDAProxy;
struct LAMBDAInstance;
struct LAMBDAMaterial;



/* Returns a pointer to a new lambda device. */
LAMBDA_API LAMBDADevice *lambdaCreateDevice();

/* Free _device. */
LAMBDA_API void lambdaReleaseDevice(LAMBDADevice *_device);

/* Returns a pointer to a new scene. */
LAMBDA_API LAMBDAScene *lambdaCreateScene(LAMBDADevice *_device, char *_name);

/* Free _scene. */
LAMBDA_API void lambdaReleaseScene(LAMBDAScene *_scene);

/* Adds the _instance object to scene. */
LAMBDA_API void lambdaAttachObject(LAMBDAScene *_scene, LAMBDAInstance *_instance);

/* Remove the _instance object from _scene. */
LAMBDA_API void lambdaDetachObject(LAMBDAScene *_scene, LAMBDAInstance *_instance);

/* Returns a pointer to a new triangle mesh object. */
LAMBDA_API LAMBDATriangleMesh *lambdaCreateTriangleMesh(LAMBDADevice *_device, char *_name);

/* Free _mesh. */
LAMBDA_API void lambdaReleaseTriangleMesh(LAMBDATriangleMesh *_mesh);

/* Allocate data buffers for _mesh. */
LAMBDA_API void lambdaTriangleMeshAllocData(LAMBDATriangleMesh *_mesh, size_t _numVertices, size_t numTriangles);

enum LAMBDABuffer {
	LAMBDA_BUFFER_VERTEX,
	LAMBDA_BUFFER_TRIANGLE,
	LAMBDA_BUFFER_VERTEX_NORMAL,
	LAMBDA_BUFFER_VERTEX_TANGENT,
	LAMBDA_BUFFER_TEXTURE_COORDINATE
};

/* Set the data of a buffer inside _mesh. */
LAMBDA_API void lambdaTriangleMeshSetBuffer(LAMBDATriangleMesh *_mesh, LAMBDABuffer _bufferType, void *_ptr, size_t _num);

/* Create an instance proxy for a triangle mesh. */
LAMBDA_API LAMBDAProxy *lambdaCreateProxy(LAMBDATriangleMesh *_mesh);

/* Create an instance object from _proxy. */
LAMBDA_API LAMBDAInstance *lambdaCreateInstance(LAMBDAProxy *_proxy);

/* Utility for applying transform parameters to the transformation matrix _xfm. */
LAMBDA_API void lambdaMakeAffineTransform(float _xfm[12], float _position[3], float _scale[3], float _eulerAngles[3]);

/* Applies the transformation _xfm to _instance. */
LAMBDA_API void lambdaSetTransform(LAMBDAInstance *_instance, float *_xfm);

/* Sets the world-space position of _instance. */
LAMBDA_API void lambdaSetPosition(LAMBDAInstance *_instance, float _position[3]);

/* Sets the world-space scale of _instance. */
LAMBDA_API void lambdaSetScale(LAMBDAInstance *_instance, float _scale[3]);

/* Sets the world-space rotation of _instance to _eulerAngles. Angle units are radians. */
LAMBDA_API void lambdaSetEulerAngles(LAMBDAInstance *_instance, float _eulerAngles[3]);

/* Creates a new material. */
LAMBDA_API LAMBDAMaterial *lambdaCreateMaterial();

LAMBDA_API_NAMESPACE_END