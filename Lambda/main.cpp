#include <iostream>
#include <omp.h>
#include <assets/AssetImporter.h>
#include<cmath>
#include <shading/OrenNayar.h>
#include <shading/Specular.h>
#include <integrators/DirectLightingIntegrator.h>
#include <sampling/HaltonSampler.h>
#include <camera/PinholeCamera.h>
#include <camera/EnvironmentCamera.h>
#include <core/TriangleMesh.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <core/RenderCoordinator.h>

int main() {

	//Spectrum::Init();

	//Declare a scene.
	Scene scene;

	AssetImporter ai;
	ai.Import("../content/monkey.obj");
	TriangleMesh mesh;
	mesh.LoadFromImport(scene.device, ai);
	mesh.smoothNormals = true;
	Texture albedo(1,1,Colour(1,1,1));
	albedo.LoadImageFile("../content/uv_grid.png");
	OrenNayarBRDF mat(&albedo);
	mat.SetSigma(.01);
	mesh.bxdf = &mat;
	
	scene.AddObject(mesh);

	AssetImporter ai2;
	ai2.Import("../content/MeshLight.obj");
	TriangleMesh box;
	box.LoadFromImport(scene.device, ai2);
	box.bxdf = &mat;
	box.smoothNormals = false;
	
	Texture emission(1, 1, Colour(1, 1, 1));
	//emission.LoadImageFile("../content/uv_grid_2.png");
	MeshLight meshLight(&box, &emission);
	meshLight.intensity = 10000;
	
	scene.AddObject(box);
	scene.AddLight(&meshLight);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/qwantani_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1.;
	ibl.offset = Vec2(PI,0);
	ibl.diameter = scene.GetBounds().max.x - scene.GetBounds().min.x;
	scene.AddLight(&ibl);
	scene.envLight = &ibl;

	scene.Commit();

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	//Make camera;
	PinholeCamera cam(Vec3(0, 1.5, 5), 1, 1);
	cam.SetFov(1.1);
	cam.SetRotation(-PI, -PI*.05);

	//Make the integrator.
	DirectLightingIntegrator integrator(&sampler);

	Film film(512, 512);

	//Render texture.
	Texture tex(512, 512, Colour(0, 0, 0));
	//Render tile.
	RenderTile tile;
	tile.camera = &cam;
	tile.scene = &scene;
	tile.integrator = &integrator;
	tile.x = 0; tile.y = 0; tile.w = 512; tile.h = 512;
	tile.film = &film;

	//Render the image...
	RenderCoordinator::ProcessTile(tile, 50);

	//Convert film to an RGB image;
	film.ToXYZTexture(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png");
	
	return 0;
}