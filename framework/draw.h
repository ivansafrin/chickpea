// chickpea unframework
// ivan safrin, 2016
// this code is licensed under the MIT license

#ifndef sprite_h
#define sprite_h

#ifdef _WINDOWS
#include <GL/glew.h>
#endif
#include <SDL.h>
#include <SDL_opengl.h>

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define PI 3.1415926536

typedef struct {
	unsigned int texture;
	stbtt_bakedchar cdata[96];
} TTFFont;

void loadTTFFont(const char *fontFile, TTFFont *font, int fontSize);
void drawTTFText(TTFFont *font, float scale, int positionAttribute, int texCoordAttribute, char *text, int center);

GLuint loadTexture(const char *image_path);
GLuint loadShaderFromFile(const char *shaderFile, GLenum type);
GLuint createProgram(const char *vertexShaderFile, const char *fragmentShaderFile);
void drawSprite(GLuint spriteTexture, int positionAttribute, int texCoordAttribute, int index, int spriteCountX, int spriteCountY, float size);
void drawSpriteAnimation(float time, float delay, char loop, const int *frameArray, int animationSize, GLuint spriteTexture, int positionAttribute, int texCoordAttribute, int spriteCountX, int spriteCountY, float size);
void drawText(int fontTexture, int positionAttribute, int texCoordAttribute, const char *text, float size, float spacing);
void drawLineCircle(float size, int vertexCount, int positionAttribute);
void drawTexture(int texture, float width, float height, float sOffset, float tOffset, int positionAttribute, int texCoordAttribute);


STBTT_DEF void getSTBTTBakedQuad(stbtt_bakedchar *chardata, int pw, int ph, int char_index, float *xpos, float *ypos, stbtt_aligned_quad *q, int opengl_fillrule)
{
   float d3d_bias = opengl_fillrule ? 0 : -0.5f;
   float ipw = 1.0f / pw, iph = 1.0f / ph;
   stbtt_bakedchar *b = chardata + char_index;
   int round_x = STBTT_ifloor((*xpos + b->xoff) + 0.5f);
   int round_y = STBTT_ifloor((*ypos + b->yoff) + 0.5f);

   q->x0 = round_x + d3d_bias;
   q->y0 = -round_y + d3d_bias;
   q->x1 = round_x + b->x1 - b->x0 + d3d_bias;
   q->y1 = round_y + b->y1 - b->y0 + d3d_bias;

   q->s0 = b->x0 * ipw;
   q->t0 = b->y0 * iph;
   q->s1 = b->x1 * ipw;
   q->t1 = b->y1 * iph;

   *xpos += b->xadvance;
}

void drawTTFText(TTFFont *font, float scale, int positionAttribute, int texCoordAttribute, char *text, int center) {
	float x=0.0f;
	float y=0.0f;
	float *vertices = malloc(sizeof(float) * 6*2*strlen(text));
	float *texCoords = malloc(sizeof(float) * 6*2*strlen(text));
	float *pVertices = vertices;
	float *pTexCoords = texCoords;
	int numVertices = 0;
	float textWidth = 0.0f;
	float textHeight = 0.0f;
	stbtt_aligned_quad q;
	while (*text) {
		if (*text >= 32) {
			getSTBTTBakedQuad(font->cdata, 1024,1024, *text-32, &x,&y,&q,1);
			float newVertices[] = {q.x0*scale,q.y0 * scale, q.x1*scale,q.y0 * scale, q.x1*scale,q.y1 * scale, q.x0*scale,q.y0*scale, q.x1*scale,q.y1*scale, q.x0*scale,q.y1*scale};
			memcpy(pVertices, newVertices, sizeof(float) * 12);
			float newTexCoords[] = {q.s0,q.t0, q.s1,q.t0, q.s1,q.t1, q.s0,q.t0, q.s1,q.t1, q.s0,q.t1};
			memcpy(pTexCoords, newTexCoords, sizeof(float) * 12);
			
			textWidth =  q.x1*scale;
			if(fabs(q.y0*scale) > textHeight) {
				textHeight =  fabs(q.y0*scale);
			}

			numVertices += 6;
			pVertices += 12;
			pTexCoords += 12;

		}
		++text;
	}

	if(center) {
		for(int i=0; i < numVertices; i++) {
			vertices[(i*2)] -= textWidth * 0.5f;
			vertices[(i*2)+1] -= textHeight * 0.5f;
		}
	}

	glEnable(GL_TEXTURE_2D);
	glBindTexture(GL_TEXTURE_2D, font->texture);

	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(positionAttribute);
	
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glEnableVertexAttribArray(texCoordAttribute);
	
	glDrawArrays(GL_TRIANGLES, 0, numVertices);

	free(vertices);
	free(texCoords);

}


void loadTTFFont(const char *fontFile, TTFFont *font, int fontSize) {
	unsigned char *ttf_buffer = malloc(1<<20);
	FILE *f = fopen(fontFile, "rb");
	fread(ttf_buffer, 1, 1<<20, f);

	int textureSize = fontSize * 16;
	unsigned char *temp_bitmap = malloc(textureSize*textureSize);

	stbtt_BakeFontBitmap(ttf_buffer,0, (float)fontSize, temp_bitmap,textureSize,textureSize, 32,96, font->cdata);
	free(ttf_buffer);
	glGenTextures(1, &font->texture);
	glBindTexture(GL_TEXTURE_2D, font->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, textureSize,textureSize, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	free(temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fclose(f);
}

GLuint loadTexture(const char *image_path) {
	int x,y,n;
	stbi_set_flip_vertically_on_load(0);
	stbi_uc *data = stbi_load(image_path, &x, &y, &n, 4);
	if(!data) {
		printf("Error reading image data: %s\n", image_path);
		return 0;
	}
	GLuint textureID;
	glGenTextures(1, &textureID);
	glBindTexture(GL_TEXTURE_2D, textureID);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, x, y, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	return textureID;
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

void drawTexture(int texture, float width, float height, float sOffset, float tOffset, int positionAttribute, int texCoordAttribute) {
	glBindTexture(GL_TEXTURE_2D, texture);

	float texCoords[] = {
		sOffset, tOffset+1.0,
		sOffset+1.0, tOffset,
		sOffset, tOffset,
		sOffset+1.0, tOffset,
		sOffset, tOffset+1.0,
		sOffset+1.0, tOffset+1.0
	};

	float vertices[] = {-0.5f * width, -0.5f  * height, 0.5f  * width, 0.5f  * height, -0.5f  * width, 0.5f  * height, 0.5f  * width, 0.5f  * height,  -0.5f  * width, -0.5f  * height, 0.5f  * width, -0.5f  * height};
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(positionAttribute);
	
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glEnableVertexAttribArray(texCoordAttribute);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
}

void drawSprite(GLuint spriteTexture, int positionAttribute, int texCoordAttribute, int index, int spriteCountX, int spriteCountY, float size) {
	
	float u = (float)(((int)index) % spriteCountX) / (float) spriteCountX;
	float v = (float)(((int)index) / spriteCountX) / (float) spriteCountY;
	float spriteWidth = 1.0/(float)spriteCountX;
	float spriteHeight = 1.0/(float)spriteCountY;
	
	float texCoords[] = {
		u, v+spriteHeight,
		u+spriteWidth, v,
		u, v,
		u+spriteWidth, v,
		u, v+spriteHeight,
		u+spriteWidth, v+spriteHeight
	};
	
	float vertices[] = {-0.5f * size, -0.5f  * size, 0.5f  * size, 0.5f  * size, -0.5f  * size, 0.5f  * size, 0.5f  * size, 0.5f  * size,  -0.5f  * size, -0.5f  * size, 0.5f  * size, -0.5f  * size};
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(positionAttribute);
	
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glEnableVertexAttribArray(texCoordAttribute);
	
	glBindTexture(GL_TEXTURE_2D, spriteTexture);
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
}

void drawSpriteAnimation(float time, float delay, char loop, const int *frameArray, int animationSize, GLuint spriteTexture, int positionAttribute, int texCoordAttribute, int spriteCountX, int spriteCountY, float size) {
	
	int frameIndex = ((int)(time/delay));
	if(!loop && frameIndex > animationSize-1) {
		frameIndex = animationSize-1;
	}
	drawSprite(spriteTexture, positionAttribute, texCoordAttribute, frameArray[frameIndex % animationSize], spriteCountX, spriteCountY, size);
}

void drawText(int fontTexture, int positionAttribute, int texCoordAttribute, const char *text, float size, float spacing) {
	float texture_size = 1.0/16.0f;
	int len = (unsigned long) strlen(text);
	float *vertexData = malloc(sizeof(float) * 12 * len);
	float *texCoordData = malloc(sizeof(float) * 12 * len);
	
	for(int i=0; i < len; i++) {
		float texture_x = (float)(((int)text[i]) % 16) / 16.0f;
		float texture_y = (float)(((int)text[i]) / 16) / 16.0f;
		
		float newData[12] = {
			((size+spacing) * i) + (-0.5f * size), 0.5f * size,
			((size+spacing) * i) + (-0.5f * size), -0.5f * size,
			((size+spacing) * i) + (0.5f * size), 0.5f * size,
			((size+spacing) * i) + (0.5f * size), -0.5f * size,
			((size+spacing) * i) + (0.5f * size), 0.5f * size,
			((size+spacing) * i) + (-0.5f * size), -0.5f * size,
		};
		memcpy(vertexData+(i*12), newData, sizeof(float) * 12);
		
		float newTexData[12] = {
			texture_x, texture_y,
			texture_x, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x + texture_size, texture_y + texture_size,
			texture_x + texture_size, texture_y,
			texture_x, texture_y + texture_size
		};
		memcpy(texCoordData+(i*12), newTexData, sizeof(float) * 12);
	}
	
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoordData);
	glEnableVertexAttribArray(texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, fontTexture);
	glDrawArrays(GL_TRIANGLES, 0, len * 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
	
	free(vertexData);
	free(texCoordData);
}

void drawLineCircle(float size, int vertexCount, int positionAttribute) {
	float *vertexData = (float*) malloc(vertexCount * 2 * sizeof(float));
	for(int i=0; i < vertexCount; i++) {
		float angle = ((PI*2.0)/(float)vertexCount) * i;
		vertexData[(i*2)] = sin(angle) * size;
		vertexData[(i*2)+1] = cos(angle) * size;
	}
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glDrawArrays(GL_LINE_LOOP, 0, vertexCount);
	glDisableVertexAttribArray(positionAttribute);
	free(vertexData);
}


#endif
