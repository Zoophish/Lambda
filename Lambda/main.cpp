#include <assets/AssetImporter.h>
#include <shading/graph/GraphInputs.h>
#include <shading/graph/GraphBxDF.h>
#include <shading/graph/GraphConverters.h>
#include <shading/graph/GraphTexture.h>
#include <shading/Fresnel.h>
#include <shading/MicrofacetDistribution.h>
#include <integrators/DirectLightingIntegrator.h>
#include <integrators/PathIntegrator.h>
#include <integrators/VolumetricPathIntegrator.h>
#include <integrators/UtilityIntegrators.h>
#include <sampling/HaltonSampler.h>
#include <camera/Camera.h>
#include <core/TriangleMesh.h>
#include <core/Instance.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <lighting/Portal.h>
#include <render/MosaicRenderer.h>
#include <utility/Memory.h>
#include <image/processing/PostProcessing.h>
#include <shading/media/HomogeneousMedium.h>
#include <shading/media/HenyeyGreenstein.h>

using namespace lambda;

int main() {
	ResourceManager resources;
	Scene scene;

	Texture red(1, 1, Colour(1, .1, .1));
	Texture yellow(1, 1, Colour(.9, .6, .6));
	Texture white(1, 1, Colour(1, 1, 1));
	Texture blue(1, 1, Colour(.63, .1, 1));
	Texture grid; grid.LoadImageFile("../content/uv_grid.png"); grid.interpolationMode = InterpolationMode::INTERP_NEAREST;
	Texture checker; checker.LoadImageFile("../content/checker.jpg"); checker.interpolationMode = InterpolationMode::INTERP_NEAREST;
	BeckmannDistribution d;
	FresnelDielectric fres(2.5);

	MemoryArena graphArena;
	namespace sg = ShaderGraph;
	sg::RGBInput *whiteNode = graphArena.New<sg::RGBInput>(Colour(1, 1, 1));
	sg::RGBInput *greenNode = graphArena.New<sg::RGBInput>(Colour(.1, 1, .1));
	sg::ImageTextureInput *gridNode = graphArena.New<sg::ImageTextureInput>(&grid);
	sg::ImageTextureInput *checkNode = graphArena.New<sg::ImageTextureInput>(&checker);
	Texture scratches(1,1, Colour(.5,.5,.5));// scratches.LoadImageFile("D:\\Assets\\POLIIGON Surface Imperfections\\Stains Liquid\\StainsLiquidGeneric003_OVERLAY_VAR1_HIRES.jpg");

	//Perlin noise texture!
	sg::Textures::Checker *perlinNoise = graphArena.New<sg::Textures::Checker>(20);
	sg::Converter::ScalarToColour *scalarToColour = graphArena.New<sg::Converter::ScalarToColour>(&perlinNoise->outputSockets[0]);

	sg::ScalarInput *sigmaNode = graphArena.New<sg::ScalarInput>(.0001);
	sg::Vec2Input *sigma2Node = graphArena.New<sg::Vec2Input>(Vec2(.1, .001));
	sg::ScalarInput *iorNode = graphArena.New<sg::ScalarInput>(1.3);
	sg::OrenNayarBRDFNode *mat = graphArena.New<sg::OrenNayarBRDFNode>(&greenNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::OrenNayarBRDFNode *mat2 = graphArena.New<sg::OrenNayarBRDFNode>(&gridNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::OrenNayarBTDFNode *matT = graphArena.New<sg::OrenNayarBTDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::FresnelBSDFNode *fresMat = graphArena.New<sg::FresnelBSDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::SpecularBRDFNode *specMat = graphArena.New<sg::SpecularBRDFNode>(&whiteNode->outputSockets[0], &fres);


	sg::MicrofacetBRDFNode *microfacetBRDF = graphArena.New<sg::MicrofacetBRDFNode>(&whiteNode->outputSockets[0], &sigmaNode->outputSockets[0], &d, &fres);

	sg::MixBxDFNode *mixMat = graphArena.New<sg::MixBxDFNode>(&fresMat->outputSockets[0], &mat->outputSockets[0], &perlinNoise->outputSockets[0]);

	Real volC[3] = { 5, 3, 4 };
	Spectrum volS = Spectrum::FromRGB(volC) * .5;

	Medium *med = new HomogeneousMedium(Spectrum(2), Spectrum(40));
	HenyeyGreenstein *phase = new HenyeyGreenstein;
	phase->g = -.2;
	med->phase = phase;

	Material material;
	material.bxdf = mixMat;

	Material material2;
	material2.bxdf = mat2;

	AssetImporter ai2;
	ai2.Import("../content/sphere.obj");
	ai2.PushToResourceManager(&resources, ImportOptions::IMP_MESHES);
	for (auto &it : resources.objectPool.pool) {
		//Material *m = MaterialImport::GetMaterial(ai2.scene, &resources, it.first);
		//if (m) {
			it.second->material = &material;
			//it.second->material->bxdf = matT;
			//it.second->material->mediaBoundary.interior = nullptr;
			scene.AddObject(it.second);
		//}
	}

	scene.hasVolumes = false;

	AssetImporter ai;
	ai.Import("../content/plane.obj");
	ai.PushToResourceManager(&resources, ImportOptions::IMP_MESHES);
	TriangleMesh plane;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &plane);
	plane.material = &material2;
	
	scene.AddObject(&plane);


	//ai.Import("../content/circleLight.obj");
	//ai.PushToResourceManager(&resources);
	//TriangleMesh lightMesh;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lightMesh);
	//lightMesh.smoothNormals = false;
	//MeshLight light(&lightMesh);
	//sg::ScalarInput temp1(3500);
	//sg::BlackbodyInput blckInpt1(&temp1.outputSockets[0]);
	//light.emission = &blckInpt1.outputSockets[0];
	//light.intensity = 650;
	//scene.AddLight(&light);
	//scene.AddObject(&lightMesh);
	
	//ai.Import("../content/SideLight.obj");
	//ai.PushToResourceManager(&resources);
	//TriangleMesh lightMesh2;
	//MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[1], &lightMesh2);
	//MeshLight light2(&lightMesh2);
	//sg::ScalarInput temp(10000);
	//sg::BlackbodyInput blckInpt(&temp.outputSockets[0]);
	//light2.emission = &blckInpt.outputSockets[0];
	//light2.intensity = 300;
	//scene.AddLight(&light2);
	//scene.AddObject(&lightMesh2);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_NEAREST;
	envMap.LoadImageFile("..\\content\\veranda_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1;
	ibl.offset = Vec2(PI*0, 0);
	scene.AddLight(&ibl);
	scene.envLight = &ibl;

	scene.Commit();

	//Set up sampler.
	Texture blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	sampleShifter.maskDimensionStart = 3;
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	CircularAperture aperture2(.05);
	ThinLensCamera cam(Vec3(0, 2, 10), 16, 9, 10, &aperture2);
	aperture2.size = .03;
	aperture2.sampler = &sampler;
	cam.focalLength = 10;
	cam.SetFov(.25);
	cam.SetRotation(-PI, -PI*.035);

	//Make some integrators.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);
	VolumetricPathIntegrator volPathIntegrator(&sampler);
	NormalPass normalPass(&sampler);
	Film film(1280, 720);

	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &pathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 4;
	renderDirective.tileSizeX = 32;
	renderDirective.tileSizeY = 32;

	AsyncMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp);
	auto start = std::chrono::system_clock::now();
	rdr.Render();
	auto end = std::chrono::system_clock::now();
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << std::endl << "TIME: " << elapsed_seconds.count();

	Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(0, 0, 0));
	//Convert film to an RGB image;
	film.ToRGBTexture(&tex);


	//Save to file.
	tex.SaveToImageFile("out.png", true, false);
	system("pause");
	return 0;
}