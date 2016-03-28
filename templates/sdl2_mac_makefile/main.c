
#include <chickpea.h>

#define X_RES 1920
#define Y_RES 1080

int main(int argc, char **argv) {

	ChickpeaWindow window;
	systemInit(&window, "Hello Chickpea", X_RES, Y_RES, 0);	

	TexturedShader shader;
	loadShaderTextured(&shader, "textured_vertex.glsl", "textured_fragment.glsl");

	int emojiTexture = loadTexture("emoji.png", 0, 0);

	float viewMatrix[16];
	float modelMatrix[16];
	float projectionMatrix[16];

	matrixSetIdentity(viewMatrix);
	matrixSetIdentity(modelMatrix);
	matrixSetIdentity(projectionMatrix);

	float aspect = (float)X_RES/(float)Y_RES;
	matrixSetOrthoProjection(projectionMatrix, -aspect, aspect, -1.0, 1.0, -1.0, 1.0);

	glUseProgram(shader.programID);
	glUniformMatrix4fv(shader.projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(shader.viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(shader.modelMatrixUniform, 1, GL_FALSE, modelMatrix);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SystemEvent event;
	float lastElapsed = 0;
	int done = 0;

	while (!done) {
		while (systemPollEvent(&window, &event)) {
			if (event.type == EVENT_QUIT) {
				done = 1;
			}
		}

		float ticks = (float)SDL_GetTicks()/1000.0f;
		float elapsed = ticks - lastElapsed;
		lastElapsed = ticks;

		glClear(GL_COLOR_BUFFER_BIT);
		glClearColor(0.2f, 0.2f, 0.2f, 1.0f);

		matrixSetIdentity(modelMatrix);
		matrixRoll(modelMatrix, sin(ticks * 3.0) * PI * 0.1);
		glUniformMatrix4fv(shader.modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		drawTexture(emojiTexture, 0.8, 0.8, 0.0, 0.0, shader.positionAttribute, shader.texCoordAttribute);
		systemShowFrame(&window);
	}
	systemShutdown();
}
