#include <thread>
#include <lambda/Lambda.h>

void dummy_callback() {
	printf("UPDATE");
}

int main() {
	LAMBDA_Device *device = lambdaCreateDevice();
	LAMBDA_Scene *scene = lambdaCreateScene(device, (char *)"default_scene");

	float pos[3] = { 0,0,0 };
	LAMBDA_Camera *camera = lambdaCreateCamera(LAMBDA_CAMERA_THIN_LENS, pos, 0, 0);
	LAMBDA_Film *film = lambdaCreateFilm(512, 512);

	LAMBDA_RenderProperties *properties = lambdaCreateRenderProperties();
	properties->lightStrategy = LAMBDA_LightStrategy::LAMBDA_LIGHT_STRATEGY_POWER;
	LAMBDA_RenderDirective *directive = lambdaCreateRenderDirective(device, scene, film, camera, properties);

	lambdaCommitScene(scene);

	LAMBDA_ProgressiveRenderer *renderer = lambdaCreateProgressiveRenderer(directive);

	lambdaSetProgressiveRendererCallback(renderer, &dummy_callback);




	// render

	lambdaStartProgressiveRenderer(renderer);

	auto t = std::chrono::seconds(5);
	std::this_thread::sleep_for(t);
	
	lambdaStopProgressiveRenderer(renderer);

	lambdaReleaseDevice(device);
	return 0;
}