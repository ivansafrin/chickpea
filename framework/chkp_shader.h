// chickpea unframework
// OpenGL ES2 shader utilities
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#ifndef CHICKPEA_SHADER_H
#define CHICKPEA_SHADER_H

#include "chkp_draw.h"

typedef struct {
	GLuint programID;
	GLint modelMatrixUniform;
	GLint projectionMatrixUniform;
	GLint viewMatrixUniform;
	GLint positionAttribute;
	GLint texCoordAttribute;
} TexturedShader;

int loadShaderTextured(TexturedShader *shader, const char *vertexProgram, const char *fragmentProgram);

typedef struct {
	GLuint programID;
	GLint modelMatrixUniform;
	GLint projectionMatrixUniform;
	GLint colorUniform;
	GLint viewMatrixUniform;
	GLint positionAttribute;
} UntexturedShader;

int loadShaderUntextured(UntexturedShader *shader, const char *vertexProgram, const char *fragmentProgram);

typedef struct {
	GLuint programID;
	GLint modelMatrixUniform;
	GLint projectionMatrixUniform;
	GLint viewMatrixUniform;
	GLint positionAttribute;
	GLint texCoordAttribute;

	GLint ambientLightUniform;
	GLint lightPositionsUniform;
	GLint lightColorsUniform;
	GLint lightBrightnessUniform;
} TexturedShader2DLights;

int loadShader2DLights(TexturedShader2DLights *shader, const char *vertexProgram, const char *fragmentProgram);

#endif

#ifdef CHICKPEA_SHADER_IMPLEMENTATION

int loadShaderTextured(TexturedShader *shader, const char *vertexProgram, const char *fragmentProgram) {
	shader->programID = createProgram(vertexProgram, fragmentProgram);
	shader->modelMatrixUniform = glGetUniformLocation(shader->programID, "modelMatrix");
	shader->projectionMatrixUniform = glGetUniformLocation(shader->programID, "projectionMatrix");
	shader->viewMatrixUniform = glGetUniformLocation(shader->programID, "viewMatrix");
	shader->positionAttribute = glGetAttribLocation(shader->programID, "position");
	shader->texCoordAttribute = glGetAttribLocation(shader->programID, "texCoord");
}

int loadShaderUntextured(UntexturedShader *shader, const char *vertexProgram, const char *fragmentProgram) {
	shader->programID = createProgram(vertexProgram, fragmentProgram);
	shader->modelMatrixUniform = glGetUniformLocation(shader->programID, "modelMatrix");
	shader->projectionMatrixUniform = glGetUniformLocation(shader->programID, "projectionMatrix");
	shader->viewMatrixUniform = glGetUniformLocation(shader->programID, "viewMatrix");
	shader->positionAttribute = glGetAttribLocation(shader->programID, "position");
	shader->colorUniform = glGetUniformLocation(shader->programID, "color");
}

int loadShader2DLights(TexturedShader2DLights *shader, const char *vertexProgram, const char *fragmentProgram) {
	shader->programID = createProgram(vertexProgram, fragmentProgram);
	shader->projectionMatrixUniform = glGetUniformLocation(shader->programID, "projectionMatrix");
	shader->viewMatrixUniform = glGetUniformLocation(shader->programID, "viewMatrix");
	shader->positionAttribute = glGetAttribLocation(shader->programID, "position");
	shader->texCoordAttribute = glGetAttribLocation(shader->programID, "texCoord");
	shader->ambientLightUniform = glGetUniformLocation(shader->programID, "ambientLight");
	shader->lightPositionsUniform = glGetUniformLocation(shader->programID, "lightPositions");
	shader->lightColorsUniform = glGetUniformLocation(shader->programID, "lightColors");
	shader->lightBrightnessUniform = glGetUniformLocation(shader->programID, "lightBrightness");
	shader->modelMatrixUniform = glGetUniformLocation(shader->programID, "modelMatrix");

}

#endif
