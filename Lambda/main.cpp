#include <iostream>
#include <omp.h>
#include <assets/AssetImporter.h>
#include<cmath>
#include <shading/OrenNayar.h>
#include <shading/Lambertian.h>
#include <shading/Specular.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/PathIntegrator.h>
#include <sampling/HaltonSampler.h>
#include <camera/PinholeCamera.h>
#include <camera/EnvironmentCamera.h>
#include <core/TriangleMesh.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <core/RenderCoordinator.h>
#include <lighting/Blackbody.h>

int main() {

	//Spectrum::Init();

	//Declare a scene.
	Scene scene;

	AssetImporter ai;
	ai.Import("../content/box.obj");
	TriangleMesh mesh;
	mesh.LoadFromImport(scene.device, ai);
	mesh.smoothNormals = false;
	Texture albedo(1,1,Colour(1,1,1));
	//albedo.LoadImageFile("../content/box_tex.png");
	albedo.interpolationMode = InterpolationMode::INTERP_NEAREST;
	OrenNayarBRDF mat(&albedo);
	mat.SetSigma(.5);
	mesh.bxdf = &mat;
	
	scene.AddObject(mesh);

	AssetImporter ai2;
	ai2.Import("../content/box_light.obj");
	TriangleMesh light;
	light.LoadFromImport(scene.device, ai2);
	//light.bxdf = &mat;
	light.smoothNormals = false;
	
	Texture emission(1, 1, Colour(1, 1, 1));
	//emission.LoadImageFile("../content/uv_grid_2.png");
	MeshLight meshLight(&light);
	meshLight.emission = &emission;
	meshLight.intensity = 10;
	
	scene.AddObject(light);
	scene.AddLight(&meshLight);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/qwantani_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 0;
	ibl.offset = Vec2(PI,0);
	//scene.AddLight(&ibl);
	scene.envLight = &ibl;

	scene.Commit();

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	//Make camera;
	PinholeCamera cam(Vec3(0, 1, 5), 1, 1);
	cam.SetFov(.7);
	cam.SetRotation(-PI, 0);

	//Make the integrator.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);

	Film film(512, 512);

	//Render texture.
	Texture tex(512, 512, Colour(0, 0, 0));
	//Render tile.
	RenderTile tile;
	tile.camera = &cam;
	tile.scene = &scene;
	tile.integrator = &directIntegrator;
	tile.x = 0; tile.y = 0; tile.w = 512; tile.h = 512;
	tile.film = &film;

	//Render the image...
	RenderCoordinator::ProcessTile(tile, 10);

	//Convert film to an RGB image;
	film.ToXYZTexture(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png");
	
	return 0;
}