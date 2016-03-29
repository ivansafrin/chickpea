// chickpea unframework
// OpenGL ES2 mesh utilities
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#include <stdio.h>
#include <stdint.h>

#ifndef CHICKPEA_MESH_H
#define CHICKPEA_MESH_H


typedef struct {
	uint32_t numVertices;
	float *positions;
	float *normals;
	float *texCoords;

	uint32_t *indices;
	uint32_t numIndices;
} CHKP_Mesh;

uint8_t loadMeshOBJ(CHKP_Mesh *mesh, const char *fileName);
void renderMesh(CHKP_Mesh *mesh, int32_t positionAttribute, int32_t texCoordAttribute, int32_t normalAttribute);

#endif

#ifdef CHICKPEA_MESH_IMPLEMENTATION

size_t chckp_getline(char **lineptr, size_t *n, FILE *stream) {
	char *bufptr = NULL;
	char *p = bufptr;
	size_t size;
	int c;

	if (lineptr == NULL) {
		return -1;
	}
	if (stream == NULL) {
		return -1;
	}
	if (n == NULL) {
		return -1;
	}
	bufptr = *lineptr;
	size = *n;

	c = fgetc(stream);
	if (c == EOF) {
		return -1;
	}
	if (bufptr == NULL) {
		bufptr = malloc(128);
		if (bufptr == NULL) {
			return -1;
		}
		size = 128;
	}
	p = bufptr;
	while (c != EOF) {
		if ((p - bufptr) > (size - 1)) {
			size = size + 128;
			bufptr = realloc(bufptr, size);
			if (bufptr == NULL) {
				return -1;
			}
		}
		*p++ = c;
		if (c == '\n') {
			break;
		}
		c = fgetc(stream);
	}

	*p++ = '\0';
	*lineptr = bufptr;
	*n = size;

	return p - bufptr - 1;
}

uint8_t loadMeshOBJ(CHKP_Mesh *mesh, const char *fileName) {

	FILE *fp = fopen(fileName, "r");
	if(!fp) {
		return 0;
	}
	char * line = NULL;
	size_t len = 0;
	
	memset(mesh, 0, sizeof(CHKP_Mesh));

	float *positions = NULL;
	uint32_t numPositions = 0;

	float *normals = NULL;
	uint32_t numNormals = 0;

	float *texCoords = NULL;
	uint32_t numTexCoords = 0;

	int *indices = NULL;
	uint32_t numIndices = 0;

	while (chckp_getline(&line, &len, fp) != -1) {
		char * pch;
		pch = strtok(line," ");

		// vertices
		if(strcmp(pch, "v") == 0) {
			pch = strtok(NULL," ");
			float v[3];
			memset(v, 0, sizeof(v));
			int idx = 0;
			while(pch && idx < 3) {
				v[idx] = atof(pch);
				pch = strtok(NULL," ");
				idx++;
			}
			if(!positions) {
				positions = malloc(sizeof(v));
			} else {
				positions = realloc(positions, (numPositions+1) * 3 * sizeof(float));
			}
			positions[numPositions*3] = v[0];
			positions[(numPositions*3)+1] = v[1];
			positions[(numPositions*3)+2] = v[2];
			++numPositions;
		// normals
		} else if(strcmp(pch, "vn") == 0) {
			pch = strtok(NULL," ");
			float v[3];
			memset(v, 0, sizeof(v));
			int idx = 0;
			while(pch && idx < 3) {
				v[idx] = atof(pch);
				pch = strtok(NULL," ");
				idx++;
			}
			if(!normals) {
				normals = malloc(sizeof(v));
			} else {
				normals = realloc(normals, (numNormals+1) * 3 * sizeof(float));
			}
			normals[numNormals*3] = v[0];
			normals[(numNormals*3)+1] = v[1];
			normals[(numNormals*3)+2] = v[2];
			++numNormals;

		// texture coordinates
		} else if(strcmp(pch, "vt") == 0) {
			pch = strtok(NULL," ");
			float v[2];
			memset(v, 0, sizeof(v));
			int idx = 0;
			while(pch && idx < 2) {
				v[idx] = atof(pch);
				pch = strtok(NULL," ");
				idx++;
			}
			if(!texCoords) {
				texCoords = malloc(sizeof(v));
			} else {
				texCoords = realloc(texCoords, (numTexCoords+1) * 2 * sizeof(float));
			}
			texCoords[numTexCoords*2] = v[0];
			texCoords[(numTexCoords*2)+1] = v[1];
			++numTexCoords;

		// indices
		} else if(strcmp(pch, "f") == 0) {
			pch = strtok(NULL," ");
			uint32_t v[3][3];
			memset(v, 0, sizeof(v));

			char sindices[3][128];
			memset(sindices, 0, sizeof(indices));
			int idx = 0;
			while(pch && idx < 3) {
				int vi[3];
				if(strlen(pch) < 128) {
					strcpy(sindices[idx], pch);
				}
				pch = strtok(NULL," ");
				idx++;
			}
			
			for(int i=0; i < 3; i++) {
				// assume v/vt/vn indexing
				int idx = 0;
				pch = strtok(sindices[i]," /"); 
				 while(pch && idx < 3) {
					 v[i][idx] = atoi(pch);
					 idx++;
					 pch = strtok(NULL," /");
				 }

				 // support for v/vt and v indexes
				 if(idx == 2) {
					 v[i][2] = v[i][1];
				 } else if(idx == 1) {
					 v[i][1] = v[i][0];
					 v[i][2] = v[i][0];
				 }

			}

			if(!indices) {
				indices = malloc(sizeof(uint32_t) * 9);
			} else {
				indices = realloc(indices, sizeof(uint32_t) * 9 * (numIndices+1));
			}
			memcpy(indices + (numIndices*9), v, sizeof(uint32_t) * 9);
			++numIndices;
		}
	}
	
	mesh->positions = malloc(sizeof(float) * numIndices * 9);
	mesh->texCoords = malloc(sizeof(float) * numIndices * 6);
	mesh->normals = malloc(sizeof(float) * numIndices * 9);

	mesh->numVertices = numIndices * 3;

	// build final arrays
	for(int i =0; i < numIndices; i++) {
		for(int j=0; j < 3; j++) {
			uint32_t pIndex = (indices[(i*9)+(j*3)])-1;
			//printf("v: %f,%f,%f\n", positions[(pIndex*3)+0], positions[(pIndex*3)+1], positions[(pIndex*3)+2]);

			mesh->positions[(i*9) + (j*3) + 0] = positions[(pIndex*3)+0];
			mesh->positions[(i*9) + (j*3) + 1] = positions[(pIndex*3)+1];
			mesh->positions[(i*9) + (j*3) + 2] = positions[(pIndex*3)+2];

			uint32_t tIndex = (indices[(i*9)+(j*3)+1])-1;
			//printf("vt: %f,%f\n", texCoords[(tIndex*2)+0], texCoords[(tIndex*2)+1]);

			mesh->texCoords[(i*6) + (j*2) + 0] = texCoords[(tIndex*2)+0];
			mesh->texCoords[(i*6) + (j*2) + 1] = 1.0-texCoords[(tIndex*2)+1];

			uint32_t nIndex = (indices[(i*9)+(j*3)+2])-1;
			//printf("vn: %f,%f,%f\n", normals[(nIndex*3)+0], normals[(nIndex*3)+1], normals[(nIndex*3)+2]);

			mesh->normals[(i*9) + (j*3) + 0] = normals[(pIndex*3)+0];
			mesh->normals[(i*9) + (j*3) + 1] = normals[(pIndex*3)+1];
			mesh->normals[(i*9) + (j*3) + 2] = normals[(pIndex*3)+2];
		}
	}


	free(positions);
	free(normals);
	free(texCoords);
	free(indices);

	fclose(fp);
	if (line) {
		free(line);
	}
	return 1;
}

void renderMesh(CHKP_Mesh *mesh, int32_t positionAttribute, int32_t texCoordAttribute, int32_t normalAttribute) {

	if(positionAttribute > -1 && mesh->positions) {
		glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, mesh->positions);
		glEnableVertexAttribArray(positionAttribute);
	}

	if(texCoordAttribute  > -1 && mesh->texCoords) {
		glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, mesh->texCoords);
		glEnableVertexAttribArray(texCoordAttribute);
	}

	if(normalAttribute  > -1 && mesh->normals) {
		glVertexAttribPointer(normalAttribute, 3, GL_FLOAT, GL_FALSE, 0, mesh->normals);
		glEnableVertexAttribArray(normalAttribute);
	}

	glDrawArrays(GL_TRIANGLES, 0, mesh->numVertices);
}

#endif
