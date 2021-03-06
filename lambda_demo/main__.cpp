/* ---- Sam Warren 2020 ----
A small offline rendering demo app that highlights some of the features in Lambda.
It briefly runs over:
	- Fundamental setup
	- Asset importing
	- Adding stuff to a scene
	- Shader graph construction
	- Lighting
	- Scene comitting
	- Render setup
	- Saving render output
*/
#pragma once
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
#include <integrators/MISVolumetricPathIntegrator.h>
#include <integrators/UtilityIntegrators.h>
#include <sampling/HaltonSampler.h>
#include <camera/Camera.h>
#include <core/TriangleMesh.h>
#include <core/Instance.h>
#include <lighting/MeshLight.h>
#include <lighting/EnvironmentLight.h>
#include <lighting/PointLight.h>
#include <lighting/Spotlight.h>
#include <render/MosaicRenderer.h>
#include <render/ProgressiveRender.h>
#include <utility/Memory.h>
#include <image/processing/PostProcessing.h>
#include <image/processing/ToneMap.h>
#include <image/processing/Denoise.h>
#include <shading/media/HomogeneousMedium.h>
#include <shading/media/HenyeyGreenstein.h>
#include <lighting/ManyLightSampler.h>

using namespace lambda;

int main() {
	//Spectrum::Init();

	//Owns all 3D asset data
	ResourceManager resources;

	//A renderable scene
	Scene scene;

	//Setup some useful textures...
	Texture red(1, 1, Colour(1, .1, .1));
	Texture yellow(1, 1, Colour(.9, .6, .6));
	Texture white(1, 1, Colour(1, 1, 1));
	Texture blue(1, 1, Colour(.63, .1, 1));
	Texture grid;
	grid.LoadImageFile("demo_content/box_tex.png");
	grid.interpolation = InterpolationMode::INTERP_NEAREST;

	//Use a memory arena for the shader graph (optional)
	MemoryArena graphArena;

	//More practical
	namespace sg = ShaderGraph;

	//Setup some colour input nodes
	sg::RGBInput *whiteNode = graphArena.New<sg::RGBInput>(Colour(1, 1, 1));
	sg::RGBInput *redNode = graphArena.New<sg::RGBInput>(Colour(1, .1, .05));
	sg::RGBInput *greenNode = graphArena.New<sg::RGBInput>(Colour(.1, 1, .1));

	//Setup some texture nodes
	sg::ImageTextureInput *gridNode = graphArena.New<sg::ImageTextureInput>(&grid);

	//Different types of procedural texture nodes (scale=25)...
	sg::Textures::Checker *checkerNoise = graphArena.New<sg::Textures::Checker>(25);
	sg::Textures::ValueNoise *valueNoise = graphArena.New<sg::Textures::ValueNoise>(25);
	sg::Textures::PerlinNoise *perlinNoise = graphArena.New<sg::Textures::PerlinNoise>(25);
	sg::Textures::Voronoi *voronoiNoise = graphArena.New<sg::Textures::Voronoi>(25);

	//Octave noise can use any procedural texture node
	sg::Textures::OctaveNoise *octaveNoise = graphArena.New<sg::Textures::OctaveNoise>(1, 1);
	octaveNoise->noise = voronoiNoise;

	//Some scalar input nodes...
	sg::ScalarInput *scalarInput = graphArena.New<sg::ScalarInput>(.02);
	//.. that can be manipulated with mathematical nodes
	sg::Maths::ScalarMathsNode *mathsNode = graphArena.New<sg::Maths::ScalarMathsNode>(sg::Maths::ScalarOperatorType::MULTIPLY);

	//It's safer to connect sockets with ShaderGraph::Connect()...
	sg::Connect(mathsNode->inputSockets[0], scalarInput->outputSockets[0]);
	sg::Connect(mathsNode->inputSockets[1], octaveNoise->outputSockets[0]);

	//Convert the noise value to Colour type (won't have to do this in future verion)
	sg::Converter::ScalarToColour *scalarToColour = graphArena.New<sg::Converter::ScalarToColour>(&octaveNoise->outputSockets[0]);

	//Setup some more scalar values for shading
	sg::ScalarInput *sigmaNode = graphArena.New<sg::ScalarInput>(1.2);
	sg::ScalarInput *iorNode = graphArena.New<sg::ScalarInput>(1.8);
	sg::ScalarInput *roughnessNode = graphArena.New<sg::ScalarInput>(.8);

	//You can also make 2D and 3D vector nodes...
	sg::Vec2Input *sigma2Node = graphArena.New<sg::Vec2Input>(Vec2(.1, .001 ));

	//Setup some necassary shading objects
	BeckmannDistribution d;
	FresnelDielectric fres(3.5);

	sg::Maths::ScalarMathsNode *invert = graphArena.New<sg::Maths::ScalarMathsNode>(sg::Maths::ScalarOperatorType::SUBTRACT);
	sg::ScalarInput *noOne = graphArena.New<sg::ScalarInput>(1);
	sg::Connect(invert->inputSockets[0], noOne->outputSockets[0]);
	sg::Connect(invert->inputSockets[1], voronoiNoise->outputSockets[0]);

	//Example of various BxDF nodes...
	sg::OrenNayarBRDFNode *diffuse = graphArena.New<sg::OrenNayarBRDFNode>(&whiteNode->outputSockets[0], &sigmaNode->outputSockets[0]);
	sg::FresnelBSDFNode *fresBSDF = graphArena.New<sg::FresnelBSDFNode>(&whiteNode->outputSockets[0], &iorNode->outputSockets[0]);
	sg::SpecularBRDFNode *specBRDF = graphArena.New<sg::SpecularBRDFNode>(&whiteNode->outputSockets[0], &fres);

	//You can mix BxDFs using a scalar socket
	sg::FresnelInput *mixFactor = graphArena.New<sg::FresnelInput>();
	mixFactor->inputSockets[0].SetDefaultValue<Real>(2.333);
	sg::MixBxDFNode *mixMat = graphArena.New<sg::MixBxDFNode>(&diffuse->outputSockets[0], &specBRDF->outputSockets[0], &mixFactor->outputSockets[0]);

	//Setup a volumetric medium
	const Real absorb[3] = { .5,0,.5 };
	const Spectrum sigmaA = Spectrum::FromRGB(absorb);
	const Spectrum sigmaS = Spectrum(0);
	std::unique_ptr<Medium> med(new HomogeneousMedium(sigmaA, sigmaS));
	std::unique_ptr<HenyeyGreenstein> phase(new HenyeyGreenstein);
	phase->g = 0;
	med->phase = phase.get();

	//Shading property objects are kept in material objects
	Texture boxTex; boxTex.LoadImageFile("demo_content/uv_grid.png"); boxTex.interpolation = InterpolationMode::INTERP_NEAREST;
	sg::ImageTextureInput *box_tex_node = graphArena.New<sg::ImageTextureInput>(&boxTex);
	sg::MicrofacetBRDFNode *microfacetBRDF = graphArena.New<sg::MicrofacetBRDFNode>(&whiteNode->outputSockets[0], &roughnessNode->outputSockets[0], &d, &fres);
	sg::LambertianBRDFNode *box_diffuse = graphArena.New<sg::LambertianBRDFNode>(&box_tex_node->outputSockets[0]);// &sigmaNode->outputSockets[0]);
	sg::OrenNayarBRDFNode *box_diffuse2 = graphArena.New<sg::OrenNayarBRDFNode>(&box_tex_node->outputSockets[0], &sigmaNode->outputSockets[0]);

	Material diffuse_material;
	diffuse_material.bxdf = diffuse;

	Material glass_material;
	glass_material.bxdf = fresBSDF;
	glass_material.mediaBoundary.interior = med.get();

	Material gloss_material;
	gloss_material.bxdf = mixMat;

	Material diffuse_material2;
	diffuse_material2.bxdf = box_diffuse2;

	Material volume_scatter_material;
	volume_scatter_material.mediaBoundary.interior = med.get();


	//Import an asset using an AssetImporter object
	AssetImporter ai;
	ai.Import("../content/Figurine.obj");

	//Push the mesh objects to the resource manager...
	ai.PushToResourceManager(&resources, (ImportOptions)(IMP_MESHES));

	//...and add all the meshes from that asset
	for (auto &it : resources.objectPool.pool) {
		//it.second->material = MaterialImport::GetMaterial(ai.scene, &resources, it.first);
		it.second->material = &glass_material;
		scene.AddObject(it.second);
	}

	//InstanceProxy proxy(resources.objectPool.pool["lucy_1"]);
	//proxy.Commit(scene.device);
	//Instance lucyInstance(&proxy);
	//lucyInstance.material = &diffuse_material;
	//lucyInstance.SetEulerAngles({0,0,0});
	//lucyInstance.Commit(scene.device);
	//scene.AddObject(&lucyInstance);

	//Let the integrators know if there are volumetrics in the scene
	scene.hasVolumes = true;

	//Import another asset file
	ai.Import("../content/Backdrop.obj");

	//You can manually make a new mesh object that isn't owned...
	TriangleMesh plane;
	//...and then manually load vertex data into from Assimp scene class
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &plane);
	plane.material = &diffuse_material2;
	plane.smoothNormals = false;
	
	//Add it to the scene
	scene.AddObject(&plane);

	//Setup a mesh light in a similar way
	ai.Import("../content/box_light.obj");
	TriangleMesh lightMesh;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &lightMesh);
	lightMesh.smoothNormals = false;

	ai.Import("../content/cube_cup_monkey.obj");
	TriangleMesh boundsMesh;
	MeshImport::LoadMeshVertexBuffers(ai.scene->mMeshes[0], &boundsMesh);
	boundsMesh.material = &diffuse_material;

	//scene.AddObject(&boundsMesh);

	//Release the asset importer to save some memory
	ai.Release();

	//The mesh will need a material still
	Material light_material;
	lightMesh.material = &light_material;

	//Construct a light from the light mesh.
	//The light will automatically be assigned the material of the mesh object.
	MeshLight light(&lightMesh);
	//Spotlight light({ 0,1.8,0 }, {0,-1,0}, PI * .3, PI * .2);
	//PointLight light;
	//light.position = { 0,1.8,0 };

	//Setup the light's emission using a shader graph...
	sg::ScalarInput *temp1 = graphArena.New<sg::ScalarInput>(3500);
	sg::BlackbodyInput *blckInpt1 = graphArena.New<sg::BlackbodyInput>(&temp1->outputSockets[0]);

	//Set the lights emission socket to the blackbody output socket
	light.emission = &blckInpt1->outputSockets[0];
	//Give the light an intensity (will be shader graph compatible in future)
	light.intensity = 8;

	//Add it to the scene - the associated light object will be added to scene.lights automatically
	//scene.AddLight(&light);
	scene.AddObject(&lightMesh);

	//Make environment lighting
	Texture envMap;
	envMap.interpolation = InterpolationMode::INTERP_NEAREST;
	envMap.LoadImageFile("../content/veranda_2k.hdr");
	//Shader graph currently not supported on environment lights
	EnvironmentLight ibl(&envMap);
	ibl.intensity = 4;
	ibl.offset = Vec2(PI*.7, 0);

	//Add it to scene's lights
	scene.AddLight(&ibl);

	//Setup a light sampler
	//ManyLightSampler lightSampler(scene, .1, false);
	PowerLightSampler lightSampler(scene);
	scene.lightSampler = &lightSampler;

	//Commit all changes to scene so it can be renderered
	scene.Commit();

	//Set up a sampler for MC...
	HaltonSampler sampler;
	//...and provide a sample shifter to the sampler
	Texture blueNoise;
	blueNoise.LoadImageFile("demo_content/HDR_RGBA_7.png");
	SampleShifter sampleShifter(&blueNoise);
	//Change the starting dimension to one that is more likely to show up as aliasing
	sampleShifter.maskDimensionStart = 3;
	sampler.sampleShifter = &sampleShifter;

	//Make a film that can be rendered to
	Film film(1024, 1024);


	//Construct a camera with a circular aperture of size .03 world units
	CircularAperture aperture2(0.15);
	ThinLensCamera cam(Vec3(0, 1, 10), film.filmData.GetWidth(), film.filmData.GetHeight(), 10, &aperture2);
	//Set focus to 10 units infront of camera
	cam.focalLength = 10;
	cam.SetFov(.22);
	cam.SetRotation(PI, -.013);

	//Make some integrators and provide them a sampler
	DirectLightingIntegrator directIntegrator(&sampler);
	PathIntegrator pathIntegrator(&sampler);
	VolumetricPathIntegrator volPathIntegrator(&sampler);
	MISVolumetricPathIntegrator misVolPathIntegrator(&sampler);
	NormalPass normalRdr(&sampler);

	//Make a render directive which encapsulates rendering properties
	RenderDirective renderDirective;
	renderDirective.scene = &scene;
	renderDirective.camera = &cam;
	renderDirective.integrator = &volPathIntegrator;
	renderDirective.film = &film;
	renderDirective.sampler = &sampler;
	renderDirective.sampleShifter = &sampleShifter;
	renderDirective.spp = 1;
	renderDirective.tileSizeX = 32;
	renderDirective.tileSizeY = 32;

	std::cout << std::endl << "Render at what spp?";
	std::cin >> renderDirective.spp;

	Texture colourPass(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(1, 1, 1, 1));

	//Render the render directive using a renderer and a tile renderer
	//ProgressiveRender rdr(renderDirective);
	//rdr.Init();
	TBBMosaicRenderer rdr(renderDirective, TileRenderers::UniformSpp);
	auto start = std::chrono::system_clock::now();
	rdr.Render();
	using namespace std::chrono_literals;
	//std::this_thread::sleep_for(12000ms);
	//rdr.Stop();
	auto end = std::chrono::system_clock::now();
	film.ToRGBTexture(&colourPass);

	//film.Clear();
	//Texture albedoPass(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(1, 1, 1, 1));
	//renderDirective.integrator = &albedoRdr;
	//rdr = TBBMosaicRenderer(renderDirective, TileRenderers::UniformSpp);
	//rdr.Render();
	//film.ToRGBTexture(&albedoPass);

	film.Clear();
	Texture normalPass(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(1, 1, 1, 1));
	renderDirective.integrator = &normalRdr;
	rdr = TBBMosaicRenderer(renderDirective, TileRenderers::UniformSpp);
	rdr.Render();
	film.ToRGBTexture(&normalPass);

	//Display render time
	std::chrono::duration<double> elapsed_seconds = end - start;
	std::cout << std::endl << "TIME: " << elapsed_seconds.count();

	//Make an RGB texture tha can be saved / displayed as an image
	//Texture tex(film.filmData.GetWidth(), film.filmData.GetHeight(), Colour(1, 1, 1, 1));	

	////Denoise the render with OIDN
	colourPass.SaveToImageFile("demo_render.png", true, false);

	PostProcessing::Denoise denoiser;
	denoiser.SetData(&colourPass);
	denoiser.Process(&colourPass);

	//Save to file - gamma=true, alpha=false
	//Different image formats can be used by changing the postfix
	colourPass.SaveToImageFile("demo_render_dn.png", true, false);
	//normalPass.SaveToImageFile("normals.png", false, false);
	system("pause");
	//rdr.outputTexture.SaveToImageFile("progressive.png", true, false);
	return 0;
}