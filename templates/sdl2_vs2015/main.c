
#include <chickpea.h>

#define X_RES 1920
#define Y_RES 1080

int main(int argc, char **argv) {

	ChickpeaWindow window;
	systemInit(&window, "Hello Chickpea", X_RES, Y_RES, 0, 0);	

	TexturedShader shader;
	loadShaderTextured(&shader, "textured_vertex.glsl", "textured_fragment.glsl");

	int emojiTexture = loadTexture("emoji.png", 0, 0);

	MVPMatrices mvp;
	initMVP(&mvp);
	float aspect = (float)X_RES/(float)Y_RES;
	matrixSetOrthoProjection(mvp.projectionMatrix, -aspect, aspect, -1.0, 1.0, -1.0, 1.0);
	setShaderMVP(&mvp, shader.programID, shader.modelMatrixUniform, shader.viewMatrixUniform, shader.projectionMatrixUniform);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	int done = 0;
	SystemEvent event;
	while (!done) {
		while (systemPollEvent(&window, &event)) {
			if (event.type == EVENT_QUIT) {
				done = 1;
			}
		}
		glClear(GL_COLOR_BUFFER_BIT);
		drawTexture(emojiTexture, 0.8, 0.8, 0.0, 0.0, shader.positionAttribute, shader.texCoordAttribute);
		systemShowFrame(&window);
	}
	systemShutdown();
}
