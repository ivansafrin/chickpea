#ifdef _WINDOWS
#include <GL/glew.h>
#endif

#include <SDL.h>
#include <SDL_opengl.h>

#define CHICKPEA_PARTICLE_IMPLEMENTATION
#include <chkp_particle.h>
#define CHICKPEA_DRAW_IMPLEMENTATION
#include <chkp_draw.h>
#define CHICKPEA_MATRIX_IMPLEMENTATION
#include <chkp_matrix.h>
#define CHICKPEA_UTIL_IMPLEMENTATION
#include <chkp_util.h>

#define X_RES 1920
#define Y_RES 1080

int main(int argc, char **argv) {
	
	SDL_Init(SDL_INIT_VIDEO);
	SDL_Window* displayWindow = SDL_CreateWindow("Pace Racer", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, X_RES, Y_RES, SDL_WINDOW_OPENGL);
	SDL_GLContext context = SDL_GL_CreateContext(displayWindow);
	SDL_GL_MakeCurrent(displayWindow, context);
#ifdef _WINDOWS
	glewInit();
#endif

	glViewport(0, 0, X_RES, Y_RES);

	uint32_t program = createProgram("textured_vertex.glsl", "textured_fragment.glsl");
	
	int positionAttribute = glGetAttribLocation(program, "position");
	int texCoordAttribute = glGetAttribLocation(program, "texCoord");

	int modelMatrixUniform = glGetUniformLocation(program, "modelMatrix");
	int projectionMatrixUniform = glGetUniformLocation(program, "projectionMatrix");
	int viewMatrixUniform = glGetUniformLocation(program, "viewMatrix");

	float viewMatrix[16];
	float modelMatrix[16];
	float projectionMatrix[16];

	matrixSetIdentity(viewMatrix);
	matrixSetIdentity(modelMatrix);
	matrixSetIdentity(projectionMatrix);

	float aspect = (float)X_RES/(float)Y_RES;
	matrixSetOrthoProjection(projectionMatrix, -aspect, aspect, -1.0, 1.0, -1.0, 1.0);

	glUseProgram(program);
	glUniformMatrix4fv(projectionMatrixUniform, 1, GL_FALSE, projectionMatrix);
	glUniformMatrix4fv(viewMatrixUniform, 1, GL_FALSE, viewMatrix);
	glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);

	int emojiTexture = loadTexture("emoji.png");

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	SDL_Event event;
	int done = 0;
	float lastElapsed = 0;
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT || event.type == SDL_WINDOWEVENT_CLOSE) {
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
		glUniformMatrix4fv(modelMatrixUniform, 1, GL_FALSE, modelMatrix);
		drawTexture(emojiTexture, 0.8, 0.8, 0.0, 0.0, positionAttribute, texCoordAttribute);
		SDL_GL_SwapWindow(displayWindow);
	}

	SDL_Quit();
		
}
