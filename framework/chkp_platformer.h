// chickpea unframework
// platformer game engine
// version 0.01
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin


#ifndef platformer_h
#define platformer_h

#include "chkp_vector.h"
#include "chkp_util.h"
#include "chkp_matrix.h"
#include <math.h>

#include <stdlib.h>
#include <string.h>

#include <stdio.h>
#include <stdlib.h>
#include <stddef.h>

typedef struct {
	int x;
	int y;
	char type[16];
} TilemapEntityPlacement;

typedef struct {
	Vector3 gravity;
	float friction;
	int worldSizeX;
	int worldSizeY;
	
	float tileSize;
	int numTilesX;
	int numTilesY;
	
	char **worldData;
} PlatformerWorld;

typedef struct {
	Vector3 position;
	Vector3 scale;
	Vector3 size;
	
	Vector3 acceleration;
	Vector3 velocity;
	float restitution;
	
	int isStatic;
	
	char bottomCollisionFlag;
	char topCollisionFlag;
	char leftCollisionFlag;
	char rightCollisionFlag;
	
	float modelMatrix[16];
	
} PlatformerEntity;

void updatePlatformerEntity(float elapsed, PlatformerWorld *world, PlatformerEntity *entity, PlatformerEntity *collisionEntities, int numCollisionEntities, const char *solidTiles, int numSolidTiles);
void initPlatformerEntity(PlatformerEntity *entity);
void loadTilemapFromFlare(PlatformerWorld *world, const char *fileName);
void loadTilemap(PlatformerWorld *world, const char *fileName);
void saveTilemap(PlatformerWorld *world, const char *fileName);
void createEmptyTilemap(PlatformerWorld *world, int sizeX, int sizeY);
void freeTilemap(PlatformerWorld *world);
void renderTilemap(PlatformerWorld *world, int tileTexture, int positionAttribute, int texCoordAttribute);
int boxBoxCollisionTest(Vector3 *position1, Vector3 *size1, Vector3 *position2, Vector3 *size2);

#endif

#ifdef CHICKPEA_PLATFORMER_IMPLEMENTATION

void loadTilemapFromFlare(PlatformerWorld *world, const char *fileName) {
	FILE * fp;
	char * line = NULL;
	size_t len = 0;
	
	fp = fopen(fileName, "r");
	if (!fp) {
		return;
	}
	
	int mapWidth = 0;
	int mapHeight = 0;
	while (getline(&line, &len, fp) != -1) {

		char * pch;
		pch = strtok(line,"=");
		
		if(strcmp(pch, "width") == 0) {
			pch = strtok(NULL,"=");
			mapWidth = atoi(pch);
		}
		
		if(strcmp(pch, "height") == 0) {
			pch = strtok(NULL,"=");
			mapHeight = atoi(pch);
		}
		
		if(strcmp(pch, "data") == 0) {
			if(mapWidth && mapHeight) {
				world->worldData = malloc(sizeof(char*) * mapHeight);
				world->worldSizeX = mapWidth;
				world->worldSizeY = mapHeight;
	
				for(int i = 0; i < mapHeight; ++i) {
					world->worldData[i] = malloc(mapWidth);
				}
				
				for(int i=0; i < mapHeight; i++) {
					getline(&line, &len, fp);
					pch = strtok(line,",");
					int x = 0;
					world->worldData[i][x] = atoi(pch)-1;
					x++;
					while(pch && x < mapWidth-1) {
						pch = strtok(NULL,",");
						world->worldData[i][x] = atoi(pch)-1;
						x++;
					}

				}
				
			}
		}
	}
	
	fclose(fp);
	if (line) {
		free(line);
	}
}

void loadTilemap(PlatformerWorld *world, const char *fileName) {
	
}

void saveTilemap(PlatformerWorld *world, const char *fileName) {
	
}

void createEmptyTilemap(PlatformerWorld *world, int sizeX, int sizeY) {
	world->worldSizeX = sizeX;
	world->worldSizeY = sizeY;
	world->worldData = malloc(sizeof(char*) * sizeY);
	for(int i=0; i < sizeY; i++) {
		world->worldData[i] = malloc(sizeX);
		memset(world->worldData[i], 1, sizeX);
	}
}

void freeTilemap(PlatformerWorld *world) {
	for(int i=0; i < world->worldSizeY; i++) {
		free(world->worldData[i]);
	}
	free(world->worldData);
}

void renderTilemap(PlatformerWorld *world, int tileTexture, int positionAttribute, int texCoordAttribute) {
	
	float padding = 0.001;
	
	float texture_size_x = (1.0/(float)world->numTilesX) - (padding*2);
	float texture_size_y = (1.0/(float)world->numTilesY) - (padding*2);
	
	float *vertexData = malloc(sizeof(float) * 12 * world->worldSizeX * world->worldSizeY);
	float *texCoordData = malloc(sizeof(float) * 12 * world->worldSizeX * world->worldSizeY);
	
	int i =0;
	for(int y=0; y < world->worldSizeY; y++) {
		for(int x=0; x < world->worldSizeX; x++) {
			float u = (float)(((int)world->worldData[y][x]) % world->numTilesX) / (float) world->numTilesX;
			float v = (float)(((int)world->worldData[y][x]) / world->numTilesX) / (float) world->numTilesY;
			
			u += padding;
			v += padding;
			
			float newData[12] = {
				world->tileSize * x, -world->tileSize * y,
				world->tileSize * x, (-world->tileSize * y)-world->tileSize,
				(world->tileSize * x)+world->tileSize, (-world->tileSize * y)-world->tileSize,
				world->tileSize * x, -world->tileSize * y,
				(world->tileSize * x)+world->tileSize, (-world->tileSize * y)-world->tileSize,
				(world->tileSize * x)+world->tileSize, -world->tileSize * y

			};
			memcpy(vertexData+(i*12), newData, sizeof(float) * 12);
			
			float newTexData[12] = {
				u, v,
				u, v+texture_size_y,
				u+texture_size_x, v+(texture_size_y),
				u, v,				
				u+texture_size_x, v+(texture_size_y),
				u+texture_size_x, v

			};
			memcpy(texCoordData+(i*12), newTexData, sizeof(float) * 12);
			
			i++;
		}
	}
	
	glVertexAttribPointer(positionAttribute, 2, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoordData);
	glEnableVertexAttribArray(texCoordAttribute);
	glBindTexture(GL_TEXTURE_2D, tileTexture);
	glDrawArrays(GL_TRIANGLES, 0, world->worldSizeX * world->worldSizeY * 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
	
	free(vertexData);
	free(texCoordData);
	
}

int boxBoxCollisionTest(Vector3 *position1, Vector3 *size1, Vector3 *position2, Vector3 *size2) {
	if(position1->y-size1->y/2.0f > position2->y + size2->y/2.0f
	   || position1->y+size1->y/2.0f < position2->y - size2->y/2.0f
	   || position1->x-size1->x/2.0f > position2->x + size2->x/2.0f
	   || position1->x+size1->x/2.0f < position2->x - size2->x/2.0f
	   ) {
		return 0;
	}
	return 1;
}

void initPlatformerEntity(PlatformerEntity *entity) {
	entity->scale.x = 1.0;
	entity->scale.y = 1.0;
	entity->scale.z = 1.0;
	entity->restitution = 1.0;
	entity->isStatic = 0;
}

float checkPointForGridCollisionY(PlatformerWorld *world, const char *solidTiles, int numSolidTiles, float x, float y) {
	
	int gridX = (int)(floor((x) / world->tileSize));
	int gridY = (int)(floor((-y) / world->tileSize));
	
	if(gridX < 0 || gridX > world->worldSizeX-1 || gridY < 0 || gridY > world->worldSizeY-1) {
		return 0.0f;
	}
	
	int isSolid = 0;
	
	for(int i=0; i < numSolidTiles; i++) {
		if(solidTiles[i] == world->worldData[gridY][gridX]) {
			isSolid = 1;
			break;
		}
	}
	
	if(isSolid) {
		float yCoordinate = (gridY * world->tileSize);
		return -y -yCoordinate;
	}
	return 0.0f;
}

float checkPointForGridCollisionX(PlatformerWorld *world, const char *solidTiles, int numSolidTiles, float x, float y) {
	
	int gridX = (int)(floor((x) / world->tileSize));
	int gridY = (int)(floor((-y) / world->tileSize));
	
	if(gridX < 0 || gridX > world->worldSizeX-1 || gridY < 0 || gridY > world->worldSizeY-1) {
		return 0.0f;
	}
	
	int isSolid = 0;
	
	for(int i=0; i < numSolidTiles; i++) {
		if(solidTiles[i] == world->worldData[gridY][gridX]) {
			isSolid = 1;
			break;
		}
	}
	
	if(isSolid) {
		float xCoordinate = ((gridX+1) * world->tileSize);
		return xCoordinate-x;
	}
	return 0.0f;
}

void updatePlatformerEntity(float elapsed, PlatformerWorld *world, PlatformerEntity *entity, PlatformerEntity *collisionEntities, int numCollisionEntities, const char *solidTiles, int numSolidTiles) {
	
	entity->bottomCollisionFlag = 0;
	entity->topCollisionFlag = 0;
	entity->leftCollisionFlag = 0;
	entity->rightCollisionFlag = 0;
	
	if(!entity->isStatic) {
		
		entity->velocity.x = lerp(entity->velocity.x, 0.0, world->friction * elapsed);
		
		entity->velocity.x += entity->acceleration.x * elapsed;
		entity->velocity.y += entity->acceleration.y * elapsed;
		entity->velocity.x += world->gravity.x * elapsed;
		entity->velocity.y += world->gravity.y * elapsed;
		
		entity->position.y += entity->velocity.y * elapsed;
		
		for(int i=0; i < numCollisionEntities; i++) {
			if(boxBoxCollisionTest(&entity->position, &entity->size, &collisionEntities[i].position, &collisionEntities[i].size)) {
				
				float penetration = fabs( fabs(entity->position.y - collisionEntities[i].position.y) - entity->size.y/2.0f - collisionEntities[i].size.y / 2.0f);
				if(entity->position.y > collisionEntities[i].position.y) {
					entity->position.y += penetration + 0.0000001;
					entity->bottomCollisionFlag = 1;
				} else {
					entity->position.y -= penetration + 0.0000001;
					entity->topCollisionFlag = 1;
				}
				entity->velocity.y = 0.0f;
			}
		}
		

		float adjust = checkPointForGridCollisionY(world, solidTiles, numSolidTiles, entity->position.x, entity->position.y - entity->size.y * 0.5);
		if(adjust != 0.0f) {
			entity->position.y += adjust;
			entity->velocity.y = 0.0f;
			entity->bottomCollisionFlag = 1;
		}
		
		adjust = checkPointForGridCollisionY(world, solidTiles, numSolidTiles, entity->position.x, entity->position.y + entity->size.y * 0.5);
		if(adjust != 0.0f) {
			entity->position.y += adjust - world->tileSize;
			entity->velocity.y = 0.0f;
			entity->topCollisionFlag = 1;
		}
		
		entity->position.x += entity->velocity.x * elapsed;
		for(int i=0; i < numCollisionEntities; i++) {
			if(boxBoxCollisionTest(&entity->position, &entity->size, &collisionEntities[i].position, &collisionEntities[i].size)) {
				
				float penetration = fabs( fabs(entity->position.x - collisionEntities[i].position.x) - entity->size.x/2.0f - collisionEntities[i].size.x / 2.0f);
				if(entity->position.x > collisionEntities[i].position.x) {
					entity->position.x += penetration + 0.0000001;
					entity->leftCollisionFlag = 1;
				} else {
					entity->position.x -= penetration + 0.000001;
					entity->rightCollisionFlag = 1;
				}
				entity->velocity.x = 0.0f;
			}
		}
		
		
		adjust = checkPointForGridCollisionX(world, solidTiles, numSolidTiles, entity->position.x - entity->size.x * 0.5, entity->position.y);
		if(adjust != 0.0f) {
			entity->position.x += adjust;
			entity->velocity.x = 0.0f;
			entity->leftCollisionFlag = 1;
		}
		
		adjust = checkPointForGridCollisionX(world, solidTiles, numSolidTiles, entity->position.x + entity->size.x * 0.5, entity->position.y);
		if(adjust != 0.0f) {
			entity->position.x += adjust - world->tileSize;
			entity->velocity.x = 0.0f;
			entity->rightCollisionFlag = 1;
		}
		
	}
	
	matrixSetIdentity(entity->modelMatrix);
	matrixTranslate(entity->modelMatrix, entity->position.x, entity->position.y, entity->position.z);	
	matrixScale(entity->modelMatrix, entity->scale.x, entity->scale.y, entity->scale.z);
}

#endif
