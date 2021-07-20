#include <lambda/Lambda.h>

void dummy_callback() {

}

int main() {
	LAMBDA_Device *device = lambdaCreateDevice();
	LAMBDA_Scene *scene = lambdaCreateScene(device, (char *)"default_scene");

	float pos[3] = { 0,0,0 };
	LAMBDA_Camera *camera = lambdaCreateCamera(LAMBDA_CAMERA_THIN_LENS, pos, 0, 0);
	LAMBDA_Film *film = lambdaCreateFilm(512, 512);

	LAMBDA_RenderProperties *properties = lambdaCreateRenderProperties();
	LAMBDA_RenderDirective *directive = lambdaCreateRenderDirective(device, film, camera, properties);

	lambdaSetScene(directive, scene);

	LAMBDA_ProgressiveRenderer *renderer = lambdaCreateProgressiveRenderer(directive);

	lambdaSetProgressiveRendererCallback(renderer, &dummy_callback);

	lambdaStartProgressiveRenderer(renderer);

	


	return 0;
}