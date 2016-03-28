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

GLuint loadShaderFromFile(const char *shaderFile, GLenum type);
GLuint createProgram(const char *vertexShaderFile, const char *fragmentShaderFile);

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
	return 1;
}

int loadShaderUntextured(UntexturedShader *shader, const char *vertexProgram, const char *fragmentProgram) {
	shader->programID = createProgram(vertexProgram, fragmentProgram);
	shader->modelMatrixUniform = glGetUniformLocation(shader->programID, "modelMatrix");
	shader->projectionMatrixUniform = glGetUniformLocation(shader->programID, "projectionMatrix");
	shader->viewMatrixUniform = glGetUniformLocation(shader->programID, "viewMatrix");
	shader->positionAttribute = glGetAttribLocation(shader->programID, "position");
	shader->colorUniform = glGetUniformLocation(shader->programID, "color");
	return 1;
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
	return 1;
}

GLuint loadShaderFromFile(const char *shaderFile, GLenum type) {
	FILE *f = fopen(shaderFile, "rb");
	if(!f) {
		printf("Unable to open [%s]\n", shaderFile);
	}
	fseek(f, 0, SEEK_END);
	GLint fsize = (GLint) ftell(f);
	fseek(f, 0, SEEK_SET);
	
	char *contents = malloc(fsize + 1);
	fread(contents, fsize, 1, f);
	fclose(f);
	contents[fsize] = 0;
	
	GLuint shaderID = glCreateShader(type);
	glShaderSource(shaderID, 1, (const char**)&contents, &fsize);
	glCompileShader(shaderID);
	
	GLint compileSuccess;
	glGetShaderiv(shaderID, GL_COMPILE_STATUS, &compileSuccess);
	
	if (compileSuccess == GL_FALSE) {
		GLchar messages[512];
		glGetShaderInfoLog(shaderID, sizeof(messages), 0, &messages[0]);
		printf("%s\n", messages);
	}
	
	free(contents);
	return shaderID;
}

GLuint createProgram(const char *vertexShaderFile, const char *fragmentShaderFile) {
	
	GLuint vertexShader = loadShaderFromFile(vertexShaderFile, GL_VERTEX_SHADER);
	GLuint fragmentShader = loadShaderFromFile(fragmentShaderFile, GL_FRAGMENT_SHADER);
	
	GLuint programID = glCreateProgram();
	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragmentShader);
	glLinkProgram(programID);
	
	GLint linkSuccess;
	glGetProgramiv(programID, GL_LINK_STATUS, &linkSuccess);
	if(linkSuccess == GL_FALSE) {
		printf("Error linking shader program!\n");
	}
	
	return programID;
}


#endif
