#include <assets/AssetImporter.h>
#include <shading/graph/GraphInputs.h>
#include <shading/graph/GraphBxDF.h>
#include <shading/graph/GraphConverters.h>
#include <shading/graph/GraphTexture.h>
#include <shading/graph/GraphMaths.h>
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
#include <image/processing/ToneMap.h>
#include <shading/media/HomogeneousMedium.h>
#include <shading/media/HenyeyGreenstein.h>
#include <lighting/ManyLightSampler.h>

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
	//FresnelConductor fresc()

	MemoryArena graphArena;
	namespace sg = ShaderGraph;
	sg::RGBInput *whiteNode = graphArena.New<sg::RGBInput>(Colour(1, 1, 1));
	sg::RGBInput *greenNode = graphArena.New<sg::RGBInput>(Colour(.1, 1, .1));
	sg::ImageTextureInput *gridNode = graphArena.New<sg::ImageTextureInput>(&grid);
	sg::ImageTextureInput *checkNode = graphArena.New<sg::ImageTextureInput>(&checker);
	Texture scratches(1,1, Colour(.5,.5,.5)); //scratches.LoadImageFile("D:\\Assets\\POLIIGON Surface Imperfections\\Stains Liquid\\StainsLiquidGeneric003_OVERLAY_VAR1_HIRES.jpg");
	sg::ImageTextureInput *scratchesNode = graphArena.New<sg::ImageTextureInput>(&scratches);

	sg::Textures::Checker *checkerNoise = graphArena.New<sg::Textures::Checker>(25);
	sg::Textures::ValueNoise *valueNoise = graphArena.New<sg::Textures::ValueNoise>(25);
	sg::Textures::PerlinNoise *perlinNoise = graphArena.New<sg::Textures::PerlinNoise>(25);
	sg::Textures::Voronoi *voronoiNoise = graphArena.New<sg::Textures::Voronoi>(25);
	sg::Textures::OctaveNoise *octaveNoise = graphArena.New<sg::Textures::OctaveNoise>(1, 1);
	octaveNoise->noise = voronoiNoise;
	sg::ScalarInput *scalarInput = graphArena.New<sg::ScalarInput>(.02);
	sg::Maths::ScalarMaths *mathsNode = graphArena.New<sg::Maths::ScalarMaths>(sg::Maths::ScalarOperatorType::MULTIPLY);
	sg::Connect(mathsNode->inputSockets[0], scalarInput->outputSockets[0]);
	sg::Connect(mathsNode->inputSockets[1], octaveNoise->outputSockets[0]);
	sg::Converter::ScalarToColour *scalarToColour = graphArena.New<sg::Converter::ScalarToColour>(&octaveNoise->outputSockets[0]);

	sg::ScalarInput *sigmaNode = graphArena.New<sg::ScalarInput>(.1);
	sg::Vec2Input *sigma2Node = graphArena.New<sg::Vec2Input>(Vec2(.1, .001));
	sg::ScalarInput *iorNode = graphArena.New<sg::ScalarInput>(1.5);
	sg::OrenNayarBRDFNode *mat = graphArena.New<sg::OrenNayarBRDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::OrenNayarBRDFNode *mat2 = graphArena.New<sg::OrenNayarBRDFNode>(&gridNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::OrenNayarBTDFNode *matT = graphArena.New<sg::OrenNayarBTDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::FresnelBSDFNode *fresMat = graphArena.New<sg::FresnelBSDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::SpecularBRDFNode *specMat = graphArena.New<sg::SpecularBRDFNode>(&whiteNode->outputSockets[0], &fres);


	sg::MicrofacetBRDFNode *microfacetBRDF = graphArena.New<sg::MicrofacetBRDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0], &d, &fres);

	sg::MixBxDFNode *mixMat = graphArena.New<sg::MixBxDFNode>(&microfacetBRDF->outputSockets[0], &mat->outputSockets[0], &checkerNoise->outputSockets[0]);

	Real volC[3] = { 5, 3, 4 };
	Spectrum volS = Spectrum::FromRGB(volC) * .5;

	Medium *med = new HomogeneousMedium(Spectrum(2), Spectrum(0));
	HenyeyGreenstein *phase = new HenyeyGreenstein;
	phase->g = -.2;
	med->phase = phase;

	Material material;
	material.bxdf = mat;

	Material glass_material;
	glass_material.bxdf = fresMat;

	Material material2;
	material2.bxdf = mat2;

	AssetImporter ai2;
	ai2.Import("../content/lucy.obj");
	ai2.PushToResourceManager(&resources, ImportOptions::IMP_MESHES);
	for (auto &it : resources.objectPool.pool) {
		//Material *m = MaterialImport::GetMaterial(ai2.scene, &resources, it.first);
		//if (m) {
			
			it.second->material = &glass_material;
			//it.second->material->bxdf = matT;
			//it.second->material->mediaBoundary.interior = nullptr;
			scene.AddObject(it.second);
		//}
	}

	scene.hasVolumes = false;

	AssetImporter ai;
	ai.Import("../content/Backdrop.obj");
	ai.PushToResourceManager(&resources, ImportOptions::IMP_MESHES);
	TriangleMesh plane;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &plane);
	plane.material = &material2;
	
	scene.AddObject(&plane);

	Material lightMat;
	ai.Import("../content/SpiralLight.obj");
	ai.PushToResourceManager(&resources, ImportOptions::IMP_MESHES);
	TriangleMesh lightMesh;
	lightMesh.material = &lightMat;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lightMesh);
	lightMesh.smoothNormals = false;
	MeshLight light(&lightMesh);
	sg::ScalarInput temp1(3500);
	sg::BlackbodyInput blckInpt1(&temp1.outputSockets[0]);
	light.emission = &blckInpt1.outputSockets[0];
	light.intensity = 160;
	//scene.AddObject(&lightMesh);

	//Make environment lighting.
	Texture envMap;
	envMap.interpolationMode = InterpolationMode::INTERP_NEAREST;
	envMap.LoadImageFile("..\\content\\autumn_park_2k.hdr");
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 1;
	ibl.offset = Vec2(PI*-.5, 0);
	scene.AddLight(&ibl);
	scene.envLight = &ibl;

	PowerLightSampler lightSampler(scene);
	//ManyLightSampler lightSampler(scene);
	scene.lightSampler = &lightSampler;
	scene.Commit();

	//Set up sampler.
	Texture blueNoise;
	blueNoise.LoadImageFile("../content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	sampleShifter.maskDimensionStart = 3;
	HaltonSampler sampler;
	sampler.sampleShifter = &sampleShifter;

	Film film(600, 800);

	CircularAperture aperture2(.05);
	ThinLensCamera cam(Vec3(0, 2, 10), film.filmData.GetWidth(), film.filmData.GetHeight(), 10, &aperture2);
	aperture2.size = .03;
	aperture2.sampler = &sampler;
	cam.focalLength = 10;
	cam.SetFov(.15);
	cam.SetRotation(-PI, -PI*.035);

	//Make some integrators.
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);
	VolumetricPathIntegrator volPathIntegrator(&sampler);
	NormalPass normalPass(&sampler);

	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &pathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 8;
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

	//PostProcessing::ToneMap toneMapPP;
	//toneMapPP.Process(&tex);

	//Save to file.
	tex.SaveToImageFile("out.png", true, false);
	system("pause");
	return 0;
}