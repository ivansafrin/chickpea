// chickpea unframework
// OpenGL ES2 drawing utilities
// version 0.01
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#ifndef sprite_h
#define sprite_h

#define STB_TRUETYPE_IMPLEMENTATION
#include "stb_truetype.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#define PI 3.1415926536

typedef struct {
	unsigned int texture;
	stbtt_bakedchar cdata[96];
	int textureSize;
} TTFFont;

int loadTTFFont(const char *fontFile, TTFFont *font, int fontSize);
int loadTTFFontRGBA(const char *fontFile, TTFFont *font, int fontSize, char color[3]);
float drawTTFText(TTFFont *font, float x, float y, float scale, int positionAttribute, int texCoordAttribute, char *text, int center);

GLuint loadTexture(const char *image_path, int nearest, int repeat);
GLuint loadTextureMipMap(const char *image_path, int nearest, int repeat, float anisotropy);

void drawSprite(GLuint spriteTexture, int positionAttribute, int texCoordAttribute, int index, int spriteCountX, int spriteCountY, float size);
void drawSpriteAnimation(float time, float delay, char loop, const int *frameArray, int animationSize, GLuint spriteTexture, int positionAttribute, int texCoordAttribute, int spriteCountX, int spriteCountY, float size);
void drawText(int fontTexture, int positionAttribute, int texCoordAttribute, const char *text, float size, float spacing);

void drawLineCircle(float size, int vertexCount, int positionAttribute);

void drawLineRect(float width, float height, int positionAttribute);
void drawLineRectAt(float x, float y, float width, float height, int positionAttribute);

void drawLine(float x0, float y0, float x1, float y1,  int positionAttribute);

void drawTexture(int texture, float width, float height, float sOffset, float tOffset, int positionAttribute, int texCoordAttribute);
void drawTextureAt(int texture, float x, float y, float width, float height, float sOffset, float tOffset, int positionAttribute, int texCoordAttribute);

#endif

#ifdef CHICKPEA_DRAW_IMPLEMENTATION

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

float drawTTFText(TTFFont *font, float x, float y, float scale, int positionAttribute, int texCoordAttribute, char *text, int center) {
	float *vertices = malloc(sizeof(float) * 6*2*strlen(text));
	float *texCoords = malloc(sizeof(float) * 6*2*strlen(text));
	float *pVertices = vertices;
	float *pTexCoords = texCoords;
	int numVertices = 0;
	float textWidth = 0.0f;
	float textHeight = 0.0f;
	float ax=0;
	float ay=0;
	stbtt_aligned_quad q;
	while (*text) {
		if (*text >= 32) {
			getSTBTTBakedQuad(font->cdata, font->textureSize,font->textureSize, *text-32, &ax,&ay,&q,1);
			float newVertices[] = {x+(q.x0*scale) ,y+(q.y0 * scale),x+( q.x1*scale),y+(q.y0 * scale), x+(q.x1*scale),y+(q.y1 * scale), x+(q.x0*scale),y+(q.y0*scale), x+(q.x1*scale),y+(q.y1*scale), x+(q.x0*scale),y+(q.y1*scale)};
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

	return ax*scale;
}

int loadTTFFontRGBA(const char *fontFile, TTFFont *font, int fontSize, char color[3]) {
	unsigned char *ttf_buffer = malloc(1<<20);
	FILE *f = fopen(fontFile, "rb");
	if(!f) {
		return 0;
	}
	fread(ttf_buffer, 1, 1<<20, f);

	font->textureSize = fontSize * 16;
	unsigned char *temp_bitmap = malloc(font->textureSize*font->textureSize);
	stbtt_BakeFontBitmap(ttf_buffer,0, (float)fontSize, temp_bitmap,font->textureSize,font->textureSize, 32,96, font->cdata);
	free(ttf_buffer);

	unsigned char *finalBitmap = malloc(font->textureSize*font->textureSize*4);
	for(int i=0; i < font->textureSize*font->textureSize; i++) {
		finalBitmap[(i*4)] = color[0];
		finalBitmap[(i*4)+1] = color[1];
		finalBitmap[(i*4)+2] = color[2];
		finalBitmap[(i*4)+3] = temp_bitmap[i];
	}
	free(temp_bitmap);

	glGenTextures(1, &font->texture);
	glBindTexture(GL_TEXTURE_2D, font->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, font->textureSize,font->textureSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, finalBitmap);
	free(finalBitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fclose(f);
	return 1;
}

int loadTTFFont(const char *fontFile, TTFFont *font, int fontSize) {
	unsigned char *ttf_buffer = malloc(1<<20);
	FILE *f = fopen(fontFile, "rb");
	if(!f) {
		return 0;
	}
	fread(ttf_buffer, 1, 1<<20, f);

	font->textureSize = fontSize * 16;
	unsigned char *temp_bitmap = malloc(font->textureSize*font->textureSize);
	int res = stbtt_BakeFontBitmap(ttf_buffer,0, (float)fontSize, temp_bitmap,font->textureSize,font->textureSize, 32,96, font->cdata);
	free(ttf_buffer);
	glGenTextures(1, &font->texture);
	glBindTexture(GL_TEXTURE_2D, font->texture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, font->textureSize,font->textureSize, 0, GL_ALPHA, GL_UNSIGNED_BYTE, temp_bitmap);
	free(temp_bitmap);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	fclose(f);
	return 1;
}

GLuint loadTextureMipMap(const char *image_path, int nearest, int repeat, float anisotropy) {
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

	if(nearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST_MIPMAP_NEAREST);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	}

	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAX_ANISOTROPY_EXT, anisotropy);

	if(repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	
	glGenerateMipmap(GL_TEXTURE_2D);	

	return textureID;

}

GLuint loadTexture(const char *image_path, int nearest, int repeat) {
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

	if(nearest) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	}

	if(repeat) {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	} else {
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP);
	}
	return textureID;
}

void drawTextureAt(int texture, float x, float y, float width, float height, float sOffset, float tOffset, int positionAttribute, int texCoordAttribute) {
	glBindTexture(GL_TEXTURE_2D, texture);

	float texCoords[] = {
		sOffset, tOffset+1.0,
		sOffset+1.0, tOffset,
		sOffset, tOffset,
		sOffset+1.0, tOffset,
		sOffset, tOffset+1.0,
		sOffset+1.0, tOffset+1.0
	};
	float vertices[] = {x-0.5f * width, y-0.5f  * height, x+0.5f  * width, y+0.5f  * height, x-0.5f  * width, y+0.5f  * height, x+0.5f  * width, y+0.5f  * height, x -0.5f  * width, y-0.5f  * height, x+0.5f  * width, y-0.5f  * height};
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertices);
	glEnableVertexAttribArray(positionAttribute);
	
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoords);
	glEnableVertexAttribArray(texCoordAttribute);
	
	glDrawArrays(GL_TRIANGLES, 0, 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
}

void drawTexture(int texture, float width, float height, float sOffset, float tOffset, int positionAttribute, int texCoordAttribute) {
	drawTextureAt(texture, 0.0, 0.0, width, height,  sOffset, tOffset, positionAttribute, texCoordAttribute);
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

void drawLineRect(float width, float height, int positionAttribute) {
	drawLineRectAt(0.0, 0.0, width, height, positionAttribute);
}

void drawLineRectAt(float x, float y, float width, float height, int positionAttribute) {
	float vertexData[] = {x-width*0.5,y-height*0.5,x+width*0.5,y-height*0.5, x+width*0.5, y+height*0.5, x-width*0.5, y+height*0.5};
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glDrawArrays(GL_LINE_LOOP, 0, 4);
	glDisableVertexAttribArray(positionAttribute);
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

void drawLine(float x0, float y0, float x1, float y1,  int positionAttribute) {
	float vertexData[] = {x0,y0,x1,y1};
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glDrawArrays(GL_LINE_LOOP, 0, 2);
	glDisableVertexAttribArray(positionAttribute);
}

#endif
