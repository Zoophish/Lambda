#pragma once
#include "APIUtilities.h"

LAMBDA_API_NAMESPACE_BEGIN



/* Opaque types */

struct LAMBDA_Device;
struct LAMBDA_Scene;
struct LAMBDA_TriangleMesh;
struct LAMBDA_Proxy;
struct LAMBDA_Instance;
struct LAMBDA_Film;
struct LAMBDA_Camera;
struct LAMBDA_RenderDirective;
struct LAMBDA_ProgressiveRenderer;



/* Lambda API types */
enum LAMBDAType {
	LAMBDA_DEVICE,
	LAMBDA_SCENE,
	LAMBDA_OBJECT,
	LAMBDA_TRIANGLE_MESH,
	LAMBDA_TEXTURE,
	LAMBDA_LIGHT
};



/* Returns a pointer to a new lambda device. */
LAMBDA_API LAMBDA_Device *lambdaCreateDevice();

/* Free _device. */
LAMBDA_API void lambdaReleaseDevice(LAMBDA_Device *_device);

/* Returns a pointer to a new scene. */
LAMBDA_API LAMBDA_Scene *lambdaCreateScene(LAMBDA_Device *_device, char *_name);

/* Free _scene. */
LAMBDA_API void lambdaReleaseScene(LAMBDA_Scene *_scene);

/* Adds the _instance object to scene. */
LAMBDA_API void lambdaAttachObject(LAMBDA_Scene *_scene, LAMBDA_Instance *_instance);

/* Remove the _instance object from _scene. */
LAMBDA_API void lambdaDetachObject(LAMBDA_Scene *_scene, LAMBDA_Instance *_instance);

/* Returns a pointer to a new triangle mesh object. */
LAMBDA_API LAMBDA_TriangleMesh *lambdaCreateTriangleMesh(LAMBDA_Device *_device, char *_name);

/* Free _mesh. */
LAMBDA_API void lambdaReleaseTriangleMesh(LAMBDA_TriangleMesh *_mesh);

/* Allocate data buffers for _mesh. */
LAMBDA_API void lambdaTriangleMeshAllocData(LAMBDA_TriangleMesh *_mesh, size_t _numVertices, size_t numTriangles);

enum LAMBDA_Buffer {
	LAMBDA_BUFFER_VERTEX,
	LAMBDA_BUFFER_TRIANGLE,
	LAMBDA_BUFFER_VERTEX_NORMAL,
	LAMBDA_BUFFER_VERTEX_TANGENT,
	LAMBDA_BUFFER_TEXTURE_COORDINATE
};

/* Set the data of a buffer inside _mesh. */
LAMBDA_API void lambdaTriangleMeshSetBuffer(LAMBDA_TriangleMesh *_mesh, LAMBDA_Buffer _bufferType, void *_ptr, size_t _num);

/* Create an instance proxy for a triangle mesh. */
LAMBDA_API LAMBDA_Proxy *lambdaCreateProxy(LAMBDA_TriangleMesh *_mesh);

/* Create an instance object from _proxy. */
LAMBDA_API LAMBDA_Instance *lambdaCreateInstance(LAMBDA_Proxy *_proxy);

/* Utility for applying transform parameters to the transformation matrix _xfm. */
LAMBDA_API void lambdaMakeAffineTransform(float _xfm[12], float _position[3], float _scale[3], float _eulerAngles[3]);

/* Applies the transformation _xfm to _instance. */
LAMBDA_API void lambdaSetTransform(LAMBDA_Instance *_instance, float *_xfm);

/* Sets the world-space position of _instance. */
LAMBDA_API void lambdaSetPosition(LAMBDA_Instance *_instance, float _position[3]);

/* Sets the world-space scale of _instance. */
LAMBDA_API void lambdaSetScale(LAMBDA_Instance *_instance, float _scale[3]);

/* Sets the world-space rotation of _instance to _eulerAngles. Angle units are radians. */
LAMBDA_API void lambdaSetEulerAngles(LAMBDA_Instance *_instance, float _eulerAngles[3]);

enum LAMBDA_Integrator {
	LAMBDA_INTEGRATOR_PATH,
	LAMBDA_INTEGRATOR_VOLPATH,
	LAMBDA_INTEGRATOR_DIRECT,
	LAMBDA_INTEGRATOR_ALBEDO,
	LAMBDA_INTEGRATOR_NORMAL,
	LAMBDA_INTEGRATOR_DEPTH,
};

/* Create a film render target. */
LAMBDA_API LAMBDA_Film *lambdaCreateFilm(int _width, int _height);

enum LAMBDA_CameraType {
	LAMBDA_CAMERA_THIN_LENS,
	LAMBDA_CAMERA_SPHERICAL
};

/* Create a renderable camera. */
LAMBDA_API LAMBDA_Camera *lambdaCreateCamera(LAMBDA_CameraType _cameraType, float _pos[3], float _phi, float _theta);

/* Set focal length. */
LAMBDA_API void lambdaSetCameraFocalLength(LAMBDA_Camera *_camera, float _focalLength);

/* Set aperture radius. */
LAMBDA_API void lambdaSetCameraApertureSize(LAMBDA_Camera *_camera, float _size);

/* Create a bundle of information to give a renderer.  */
LAMBDA_API LAMBDA_RenderDirective *lambdaCreateRenderDirective(LAMBDA_Device *_device, LAMBDA_Film *_film, LAMBDA_Camera *_camera, LAMBDA_Integrator _integrator, int _spp);

/* Assign the directive an integrator. */
LAMBDA_API void lambdaSetIntegrator(LAMBDA_RenderDirective *_directive, LAMBDA_Integrator _integrator);

/* Set the render tile size (default is 16x16). */
LAMBDA_API void lambdaSetTileSize(LAMBDA_RenderDirective *_directive, int _width, int _height);

/* Bind a camera to the directive. */
LAMBDA_API void lambdaBindCamera(LAMBDA_RenderDirective *_directive, LAMBDA_Camera *_camera);

/* Bind a scene to the directive. */
LAMBDA_API void lambdaBindScene(LAMBDA_RenderDirective *_directive, LAMBDA_Scene *_scene);

/* Create a progressive renderer instance. */
LAMBDA_API LAMBDA_ProgressiveRenderer *lambdaCreateProgressiveRenderer(LAMBDA_RenderDirective *_directive);

/* Sets the update callback; called everytime the render output is updated.  */
LAMBDA_API void lambdaSetProgressiveRendererCallback(LAMBDA_ProgressiveRenderer *_renderer, void(*_callback)());

/* Returns a pointer to the first RGBA32 texture element and stores _width and _height.  */
LAMBDA_API void *lambdaGetProgressiveRendererOutput(LAMBDA_ProgressiveRenderer *_renderer, int *_width, int *_height);

LAMBDA_API_NAMESPACE_END