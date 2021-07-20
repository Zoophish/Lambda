/*
Lambda API 0.1
*/

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



/* Memory-managed types */
enum LAMBDA_Type INT_ENUM {
	LAMBDA_SCENE,
	LAMBDA_TRIANGLE_MESH,
	LAMBDA_TEXTURE,
	LAMBDA_MATERIAL
};



/* Returns a pointer to a new lambda device. */
LAMBDA_API LAMBDA_Device *lambdaCreateDevice();

/* Free _device. */
LAMBDA_API void lambdaReleaseDevice(LAMBDA_Device *_device);

/* Free a resource from the device memory. */
LAMBDA_API void lambdaReleaseResource(LAMBDA_Device *_device, char *_name, LAMBDA_Type _type);

/* Returns a pointer to a new scene. */
LAMBDA_API LAMBDA_Scene *lambdaCreateScene(LAMBDA_Device *_device, char *_name);

/* Submit changes to the scene for rendering. */
LAMBDA_API void lambdaCommitScene(LAMBDA_Scene *_scene);

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

enum LAMBDA_Buffer INT_ENUM {
	LAMBDA_BUFFER_VERTEX,
	LAMBDA_BUFFER_TRIANGLE,
	LAMBDA_BUFFER_VERTEX_NORMAL,
	LAMBDA_BUFFER_VERTEX_TANGENT,
	LAMBDA_BUFFER_TEXTURE_COORDINATE
};

/* Set the data of a buffer inside _mesh. Format must be float array. */
LAMBDA_API void lambdaTriangleMeshSetBuffer(LAMBDA_TriangleMesh *_mesh, LAMBDA_Buffer _bufferType, void *_ptr, size_t _num);

/* Create an instance proxy for a triangle mesh. */
LAMBDA_API LAMBDA_Proxy *lambdaCreateProxy(LAMBDA_Device *_device, LAMBDA_TriangleMesh *_mesh);

/* Free _proxy */
LAMBDA_API void lambdaReleaseProxy(LAMBDA_Proxy *_proxy);

/* Create an instance object from _proxy. */
LAMBDA_API LAMBDA_Instance *lambdaCreateInstance(LAMBDA_Device *_device, LAMBDA_Proxy *_proxy);

LAMBDA_API void lambdaReleaseInstance(LAMBDA_Instance *_instance);

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

enum LAMBDA_Integrator INT_ENUM {
	LAMBDA_INTEGRATOR_PATH,
	LAMBDA_INTEGRATOR_VOLPATH,
	LAMBDA_INTEGRATOR_DIRECT,
	LAMBDA_INTEGRATOR_NORMAL,
	LAMBDA_INTEGRATOR_DEPTH,
	LAMBDA_INTEGRATOR_AOV
};

enum LAMBDA_LightStrategy INT_ENUM {
	LAMBDA_LIGHT_STRATEGY_POWER,
	LAMBDA_LIGHT_STRATEGY_TREE
};

/* Create a film render target. */
LAMBDA_API LAMBDA_Film *lambdaCreateFilm(LAMBDA_Device *_device, int _width, int _height);

/* Free _film */
LAMBDA_API void lambdaReleaseFilm(LAMBDA_Film *_film) {
	delete _film;
}

enum LAMBDA_CameraType INT_ENUM {
	LAMBDA_CAMERA_THIN_LENS,
	LAMBDA_CAMERA_SPHERICAL
};

/* Create a renderable camera. */
LAMBDA_API LAMBDA_Camera *lambdaCreateCamera(LAMBDA_Device *_device, LAMBDA_CameraType _cameraType, float _pos[3], float _phi, float _theta);

/* Free _camera */
LAMBDA_API void lambdaReleaseCamera(LAMBDA_Camera *_camera);

/* Set camera position. */
LAMBDA_API void lambdaSetCameraPosition(LAMBDA_Camera *_camera, float _pos[3]);

/* Set camera view direction with spherical angles. */
LAMBDA_API void lambdaSetCameraAngles(LAMBDA_Camera *_camera, float _phi, float _theta);

/* Set camera view direction axes directly. */
LAMBDA_API void lambdaSetCameraAxes(LAMBDA_Camera *_camera, float _xHat[3], float _yHat[3], float _zHat[3]);

/* Set focal length. */
LAMBDA_API void lambdaSetCameraFocalLength(LAMBDA_Camera *_camera, float _focalLength);

/* Set aperture radius. */
LAMBDA_API void lambdaSetCameraApertureSize(LAMBDA_Camera *_camera, float _size);

/*
* LAMBDA_RenderProperties:
*  spp -------------------- samples per pixel for offline rendering if applicable
*  tileSizeX, tileSizeY --- size in pixels of render tiles
*  numThreads ------------- target number of render threads. 0 = automatic
*  integrator ------------- renderng method to use
*  lightStrategy ---------- light sampling strategy to use
*/
struct LAMBDA_RenderProperties {
	unsigned spp;
	unsigned tileSizeX;
	unsigned tileSizeY;
	unsigned numThreads;
	LAMBDA_Integrator integrator;
	LAMBDA_LightStrategy lightStrategy;
};

/* Creates render properties with default values. */
LAMBDA_API LAMBDA_RenderProperties *lambdaCreateRenderProperties();

/* Creates a bundle of information complete to give to a renderer.  */
LAMBDA_API LAMBDA_RenderDirective *lambdaCreateRenderDirective(LAMBDA_Device *_device, LAMBDA_Scene *_scene, LAMBDA_Film *_film, LAMBDA_Camera *_camera, LAMBDA_RenderProperties *_properties);

/* Free _directive */
LAMBDA_API void lambdaReleaseRenderDirective(LAMBDA_RenderDirective *_directive);

/* Bind a camera to the directive. */
LAMBDA_API void lambdaSetCamera(LAMBDA_RenderDirective *_directive, LAMBDA_Camera *_camera);

/* Bind a scene to the directive. */
LAMBDA_API void lambdaSetScene(LAMBDA_RenderDirective *_directive, LAMBDA_Scene *_scene);

/* Create a progressive renderer instance. */
LAMBDA_API LAMBDA_ProgressiveRenderer *lambdaCreateProgressiveRenderer(LAMBDA_Device *_device, LAMBDA_RenderDirective *_directive);

/* Free _renderer */
LAMBDA_API void lambdaReleaseProgressiveRenderer(LAMBDA_ProgressiveRenderer *_renderer);

/* Sets the update callback; called everytime the render output is updated.  */
LAMBDA_API void lambdaSetProgressiveRendererCallback(LAMBDA_ProgressiveRenderer *_renderer, void(*_callback)());

/* Returns a pointer to the first RGBA32f render output texture pixel and stores _width and _height.  */
LAMBDA_API void *lambdaGetProgressiveRendererData(LAMBDA_ProgressiveRenderer *_renderer, int *_width, int *_height);

/* Begin rendering progressively. */
LAMBDA_API void lambdaStartProgressiveRenderer(LAMBDA_ProgressiveRenderer *_renderer);

/* Stop progressive renderering. */
LAMBDA_API void lambdaStopProgressiveRenderer(LAMBDA_ProgressiveRenderer *_renderer);

LAMBDA_API_NAMESPACE_END