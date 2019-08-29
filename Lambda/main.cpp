#include <iostream>
#include <omp.h>
#include <assets/AssetImporter.h>
#include<cmath>
#include <shading/OrenNayar.h>
#include <shading/Lambertian.h>
#include <shading/Specular.h>
#include <shading/Specular.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/PathIntegrator.h>
#include <sampling/HaltonSampler.h>
#include <camera/PinholeCamera.h>
#include <camera/EnvironmentCamera.h>
#include <core/TriangleMesh.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <lighting/MeshPortal.h>
#include <core/RenderCoordinator.h>
#include <lighting/Blackbody.h>

int main() {


	Scene scene;

	AssetImporter ai;
	ai.Import("../content/box_empty.obj");
	TriangleMesh mesh;
	mesh.LoadFromImport(scene.device, ai);
	mesh.smoothNormals = true;
	Texture albedo(1,1,Colour(1,1,1));
	albedo.LoadImageFile("../content/box_tex.png");
	albedo.interpolationMode = InterpolationMode::INTERP_NEAREST;
	OrenNayarBRDF mat(&albedo, 1.3);
	mat.SetSigma(.5);
	mesh.bxdf = &mat;
	
	scene.AddObject(mesh);

	ai.Import("../content/lucy.obj");
	TriangleMesh lucy;
	lucy.LoadFromImport(scene.device, ai);
	lucy.smoothNormals = true;
	Texture white(1, 1, Colour(1, 1, 1));
	OrenNayarBRDF mat2(&white, 1);
	lucy.bxdf = &mat2;

	scene.AddObject(lucy);

	AssetImporter ai2;
	ai2.Import("../content/box_light.obj");
	TriangleMesh light;
	light.LoadFromImport(scene.device, ai2);
	light.smoothNormals = false;
	
	Texture emission(1, 1, Colour(1, 1, 1));
	//emission.LoadImageFile("../content/uv_grid_2.png");
	MeshLight meshLight(&light);
	meshLight.emission = &emission;
	meshLight.intensity = 100;
	
	scene.AddObject(light);
	scene.AddLight(&meshLight);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_BILINEAR;
	envMap.LoadImageFile("../content/lookout_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 0;
	ibl.offset = Vec2(0, 0);
	//scene.AddLight(&ibl);
	scene.envLight = &ibl;

	//ai.Import("../content/box_front.obj");
	//TriangleMesh portalMesh;
	//portalMesh.LoadFromImport(scene.device, ai);
	//portalMesh.smoothNormals = false;
	//MeshPortal portal(&ibl, &portalMesh);
	//scene.AddLight(&portal);

	scene.Commit();

	//Set up sampler.
	TextureRGBA32 blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	//Make camera;
	PinholeCamera cam(Vec3(0, 1, 5), 1, 1);
	cam.SetFov(.52);
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
	tile.integrator = &pathIntegrator;
	tile.x = 0; tile.y = 0; tile.w = 512; tile.h = 512;
	tile.film = &film;

	//Render the image...
	RenderCoordinator::ProcessTile(tile, 4);

	//Convert film to an RGB image;
	film.ToXYZTexture(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png");
	
	return 0;
}