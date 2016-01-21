// chickpea unframework
// simple particle system
// version 0.01
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#ifndef chkp_particle_h
#define chkp_particle_h

#include <chkp_util.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>

typedef struct {
	float position[3];
	float velocity[3];
	float color[4];
	float size;
	float sizeDeviation;
	float lifetime;
} Particle;

typedef struct {
	float emitterDirection[3];
	float emitterDirectionDeviation[3];
	float emitterVelocity;
	float emitterVelocityDeviation;

	float emitterSize[3];
	float emitterPosition[3];

	float particleLifetime;

	float particleSizeDeviation;
	float particleSizeStart;
	float particleSizeEnd;
	
	float friction;

	float particleColorStart[4];
	float particleColorEnd[4];

	Particle *particles;
	uint32_t particleCount;
} ParticleSystem;

void initParticleSystem(ParticleSystem *system, uint32_t count, float lifetime);
void simulateParticleSystem(float elapsed, ParticleSystem *system);

void particleSystemGravity(float elapsed, ParticleSystem *system, float gravity[3]);
void particleSystemPointGravity(float elapsed, ParticleSystem *system, float gravity[3], float distance, float strength);
void particleSystemForceField(ParticleSystem *system, float elapsed, float position[3], float radius, float reflect);

void drawParticleSystem(ParticleSystem *system, uint32_t texture, int positionAttribute, int texCoordAttribute, int colorAttribute);
void resetParticle(ParticleSystem *system, Particle *particle);
void resetParticleSystem(ParticleSystem *system);

#endif

#ifdef CHICKPEA_PARTICLE_IMPLEMENTATION

void resetParticle(ParticleSystem *system, Particle *particle) {
	particle->lifetime = fmod(particle->lifetime, system->particleLifetime);;
	particle->size = system->particleSizeStart;
	memcpy(particle->color, system->particleColorStart, sizeof(float) * 4);
	particle->sizeDeviation = ((float)rand()/(float)RAND_MAX) * system->particleSizeDeviation;
	for(int i=0; i < 3; i++) {
		particle->velocity[i] = (system->emitterDirection[i] - (system->emitterDirectionDeviation[i]*0.5f)+system->emitterDirectionDeviation[i]*((float)rand()/(float)RAND_MAX))* (system->emitterVelocity-(system->emitterVelocityDeviation*0.5f)+system->emitterVelocityDeviation*((float)rand()/(float)RAND_MAX));
	}

	for(int i=0; i < 3; i++) {
		particle->position[i] = system->emitterPosition[i] - (system->emitterSize[i]*0.5f)+system->emitterSize[i]*((float)rand()/(float)RAND_MAX);
	}

}

void initParticleSystem(ParticleSystem *system, uint32_t count, float lifetime) {
	system->particleCount = count;
	system->particleLifetime = lifetime;
	system->particleSizeDeviation = 0.0f;

	memset(system->emitterPosition, 0, sizeof(float) * 3);
	memset(system->emitterDirection, 0, sizeof(float) * 3);
	memset(system->emitterSize, 0, sizeof(float) * 3);
	memset(system->emitterDirectionDeviation, 0, sizeof(float) * 3);
	system->emitterVelocity = 0.0f;
	
	system->friction = 0.0;

	system->particleColorStart[0] = 1.0f;
	system->particleColorStart[1] = 1.0f;
	system->particleColorStart[2] = 1.0f;
	system->particleColorStart[3] = 1.0f;
	system->particleColorEnd[0] = 1.0f;
	system->particleColorEnd[1] = 1.0f;
	system->particleColorEnd[2] = 1.0f;
	system->particleColorEnd[3] = 1.0f;
	system->particles = malloc(sizeof(Particle) * count);
	
	resetParticleSystem(system);
}

void resetParticleSystem(ParticleSystem *system) {
	for(int i=0; i < system->particleCount; i++) {
		resetParticle(system, &system->particles[i]);
		system->particles[i].lifetime = (((float)rand() / (float)RAND_MAX) * system->particleLifetime) * -1.0;
	}	
}

void particleSystemForceField(ParticleSystem *system, float elapsed, float position[3], float radius, float reflect) {
	for(int i=0; i < system->particleCount; i++) {
		float normal[3];
		for(int j=0; j < 3; j++) {
			normal[j] = system->particles[i].position[j] - position[j];
		}
		
		float tl = sqrtf( normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
		if(tl < radius) {
			float invTl = 1.0 / tl;
			for(int j=0; j < 3; j++) {
				normal[j] *= invTl;
				system->particles[i].position[j] = position[j] + (normal[j] * radius);
			}
			system->particles[i].velocity[0] += normal[0] * reflect * elapsed;
			system->particles[i].velocity[1] += normal[1] * reflect * elapsed;
		}
	}
}

void particleSystemPointGravity(float elapsed, ParticleSystem *system, float gravity[3], float distance, float strength) {

	for(int i=0; i < system->particleCount; i++) {
		if(system->particles[i].lifetime >= 0.0) {
			float naccel[3];
			for(int j=0; j < 3; j++) {
				naccel[j] = gravity[j] - system->particles[i].position[j];;
			}	
		
			float tl = sqrtf( naccel[0] * naccel[0] + naccel[1] * naccel[1] + naccel[2] * naccel[2]);
			float invTl = 1.0 / tl;

			float dAffect = distance-tl;
			if(dAffect < 0.0) {	
				dAffect = 0.0;
			}

			for(int j=0; j < 3; j++) {
				naccel[j] *= invTl;
				system->particles[i].velocity[j] += (naccel[j] * strength) * elapsed * dAffect;
			}
		}
	}
}

void particleSystemGravity(float elapsed, ParticleSystem *system, float gravity[3]) {
	for(int i=0; i < system->particleCount; i++) {
		if(system->particles[i].lifetime >= 0.0) {
			for(int j=0; j < 3; j++) {
				system->particles[i].velocity[j] += gravity[j] * elapsed;
			}
		}
	}
}

void simulateParticleSystem(float elapsed, ParticleSystem *system) {
	for(int i=0; i < system->particleCount; i++) {
		system->particles[i].lifetime += elapsed;
		if(system->particles[i].lifetime >= system->particleLifetime) {
			resetParticle(system, &system->particles[i]);
		}
		if(system->particles[i].lifetime > 0.0) {
			float relativeLifetime = system->particles[i].lifetime / system->particleLifetime;
			system->particles[i].size = lerp(system->particleSizeStart+system->particles[i].sizeDeviation, system->particleSizeEnd, relativeLifetime);
			for(int j=0; j < 3; j++) {
				system->particles[i].position[j] += system->particles[i].velocity[j] * elapsed;
				system->particles[i].velocity[j] = lerp(system->particles[i].velocity[j], 0.0, system->friction * elapsed);
			}
			for(int j=0; j < 4; j++) {
				system->particles[i].color[j] = lerp(system->particleColorStart[j], system->particleColorEnd[j], relativeLifetime);
			}
		}

		
	}
}

void drawParticleSystem(ParticleSystem *system, uint32_t texture, int positionAttribute, int texCoordAttribute, int colorAttribute) {
	float *vertexData = malloc(sizeof(float) * 18 * system->particleCount);
	float *texCoordData = malloc(sizeof(float) * 12 * system->particleCount);
	float *colorData = malloc(sizeof(float) * 24 * system->particleCount);

	int particlesToRender = 0;
	for(int i=0; i < system->particleCount; i++) {
		if(system->particles[i].lifetime > 0.0) {
		float size = system->particles[i].size;
		float newData[18] = {
			system->particles[i].position[0] - size * 0.5, system->particles[i].position[1] + size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] - size * 0.5, system->particles[i].position[1] - size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] + size * 0.5, system->particles[i].position[1] - size * 0.5, system->particles[i].position[2],

			system->particles[i].position[0] - size * 0.5, system->particles[i].position[1] + size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] + size * 0.5, system->particles[i].position[1] - size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] + size * 0.5, system->particles[i].position[1] + size * 0.5, system->particles[i].position[2]
		};
		memcpy(vertexData+(particlesToRender*18), newData, sizeof(float) * 18);
		
		float newTexData[12] = {
			0.0f, 0.0,
			0.0, 1.0f,
			1.0f, 1.0f,

			0.0f, 0.0,
			1.0f, 1.0f,
			1.0f, 0.0f,

		};
		memcpy(texCoordData+(particlesToRender*12), newTexData, sizeof(float) * 12);

		float newColorData[24] = {
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3]
		};
		memcpy(colorData+(particlesToRender*24), newColorData, sizeof(float) * 24);
 		particlesToRender++;
		}
	}

	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoordData);
	glEnableVertexAttribArray(texCoordAttribute);
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, colorData);
	glEnableVertexAttribArray(colorAttribute);

	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, particlesToRender * 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
	glDisableVertexAttribArray(colorAttribute);
	
	free(vertexData);
	free(texCoordData);
	free(colorData);
}

#endif
