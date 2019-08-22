#include <iostream>
#include <assets/AssetImporter.h>
#include<cmath>
#include <shading/OrenNayar.h>
#include <integrators/DirectLightingIntegrator.h>
#include <sampling/HaltonSampler.h>
#include <camera/PinholeCamera.h>
#include <camera/EnvironmentCamera.h>
#include <core/TriangleMesh.h>
#include <lighting/TriangleLight.h>
#include <lighting/EnvironmentLight.h>
#include <core/RenderCoordinator.h>

int main() {

	//Spectrum::Init();

	//Declare a scene.
	Scene scene;

	//Import the scene model(s).
	AssetImporter ai;
	ai.Import("../content/thai_statue.obj");
	TriangleMesh mesh;
	mesh.smoothNormals = false;
	mesh.LoadFromImport(scene.device, ai);

	Texture albedo(1,1,Colour(1,1,1));
	//albedo.LoadImageFile("../content/uv_grid.png");
	OrenNayarBRDF mat(&albedo);
	mesh.bxdf = &mat;

	//Add objects to scene.
	scene.AddObject(mesh);
	scene.Commit();

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/veranda_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.offset = Vec2(0,0);
	scene.lights.push_back(&ibl);

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_63.png");
	SampleShifter sampleShifter(&blueNoise);
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	//Make camera;
	PinholeCamera cam(Vec3(0, 2, 5), 1, 1);
	cam.SetFov(1.1);
	cam.SetRotation(-PI, 0);

	//Make the integrator.
	DirectLightingIntegrator integrator(&sampler);

	Film film(1024, 1024);

	//Render texture.
	Texture tex(1024, 1024, Colour(0, 0, 0));
	//Render tile.
	RenderTile tile;
	tile.camera = &cam;
	tile.scene = &scene;
	tile.integrator = &integrator;
	tile.x = 0; tile.y = 0; tile.w = 1024; tile.h = 1024;
	tile.film = &film;

	//Render the image...
	RenderCoordinator::ProcessTile(tile, 32);

	//Convert film to an RGB image;
	film.ToXYZTexture(&tex);
	
	//Save to file.
	tex.SaveToImageFile("out.png", true);
	
	return 0;
}