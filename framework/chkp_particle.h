
#ifndef chkp_particle_h
#define chkp_particle_h

#include <chkp_util.h>

typedef struct {
	float position[3];
	float velocity[3];
	float color[4];
	float size;
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

	float particleSizeStart;
	float particleSizeEnd;
	
	float particleColorStart[4];
	float particleColorEnd[4];

	Particle *particles;
	uint32_t particleCount;
} ParticleSystem;

void initParticleSystem(ParticleSystem *system, uint32_t count);
void simulateParticleSystem(float elapsed, ParticleSystem *system);
void particleSystemGravity(float elapsed, ParticleSystem *system, float gravity[3]);
void drawParticleSystem(ParticleSystem *system, uint32_t texture, int positionAttribute, int texCoordAttribute, int colorAttribute);
void resetParticle(ParticleSystem *system, Particle *particle);

#endif

#ifdef CHICKPEA_PARTICLE_IMPLEMENTATION

void resetParticle(ParticleSystem *system, Particle *particle) {
	particle->lifetime = fmod(particle->lifetime, system->particleLifetime);;
	particle->size = system->particleSizeStart;
	for(int i=0; i < 3; i++) {
		particle->velocity[i] = (system->emitterDirection[i] - (system->emitterDirectionDeviation[i]*0.5f)+system->emitterDirectionDeviation[i]*((float)rand()/(float)RAND_MAX))* (system->emitterVelocity-(system->emitterVelocityDeviation*0.5f)+system->emitterVelocityDeviation*((float)rand()/(float)RAND_MAX));
	}

	for(int i=0; i < 3; i++) {
		particle->position[i] = system->emitterPosition[i] - (system->emitterSize[i]*0.5f)+system->emitterSize[i]*((float)rand()/(float)RAND_MAX);
	}

}

void initParticleSystem(ParticleSystem *system, uint32_t count) {
	system->particleCount = count;
	system->particleLifetime = 1.0;

	memset(system->emitterPosition, 0, sizeof(float) * 3);
	memset(system->emitterDirection, 0, sizeof(float) * 3);
	memset(system->emitterSize, 0, sizeof(float) * 3);
	memset(system->emitterDirectionDeviation, 0, sizeof(float) * 3);
	system->emitterVelocity = 0.0f;
	
	system->particleColorStart[0] = 1.0f;
	system->particleColorStart[1] = 1.0f;
	system->particleColorStart[2] = 1.0f;
	system->particleColorStart[3] = 1.0f;
	system->particleColorEnd[0] = 1.0f;
	system->particleColorEnd[1] = 1.0f;
	system->particleColorEnd[2] = 1.0f;
	system->particleColorEnd[3] = 1.0f;

	system->particles = malloc(sizeof(Particle) * count);
	for(int i=0; i < count; i++) {
		resetParticle(system, &system->particles[i]);
		system->particles[i].lifetime = system->particleLifetime + (((float)rand() / (float)RAND_MAX) * system->particleLifetime);
	}	
}

void particleSystemGravity(float elapsed, ParticleSystem *system, float gravity[3]) {
	for(int i=0; i < system->particleCount; i++) {
		for(int j=0; j < 3; j++) {
			system->particles[i].velocity[j] += gravity[j] * elapsed;
		}
	}
}

void simulateParticleSystem(float elapsed, ParticleSystem *system) {
	for(int i=0; i < system->particleCount; i++) {
		float relativeLifetime = system->particles[i].lifetime / system->particleLifetime;
		system->particles[i].size = lerp(system->particleSizeStart, system->particleSizeEnd, relativeLifetime);
		for(int j=0; j < 3; j++) {
			system->particles[i].position[j] += system->particles[i].velocity[j] * elapsed;
		}
		for(int j=0; j < 4; j++) {
			system->particles[i].color[j] = lerp(system->particleColorStart[j], system->particleColorEnd[j], relativeLifetime);
		}

		system->particles[i].lifetime += elapsed;
		if(system->particles[i].lifetime >= system->particleLifetime) {
			resetParticle(system, &system->particles[i]);
		}
		
	}
}

void drawParticleSystem(ParticleSystem *system, uint32_t texture, int positionAttribute, int texCoordAttribute, int colorAttribute) {
	float *vertexData = malloc(sizeof(float) * 18 * system->particleCount);
	float *texCoordData = malloc(sizeof(float) * 12 * system->particleCount);
	float *colorData = malloc(sizeof(float) * 24 * system->particleCount);

	for(int i=0; i < system->particleCount; i++) {
		
		float size = system->particles[i].size;
		float newData[18] = {
			system->particles[i].position[0] - size * 0.5, system->particles[i].position[1] + size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] - size * 0.5, system->particles[i].position[1] - size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] + size * 0.5, system->particles[i].position[1] - size * 0.5, system->particles[i].position[2],

			system->particles[i].position[0] - size * 0.5, system->particles[i].position[1] + size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] + size * 0.5, system->particles[i].position[1] - size * 0.5, system->particles[i].position[2],
			system->particles[i].position[0] + size * 0.5, system->particles[i].position[1] + size * 0.5, system->particles[i].position[2]
		};
		memcpy(vertexData+(i*18), newData, sizeof(float) * 18);
		
		float newTexData[12] = {
			0.0f, 0.0,
			0.0, 1.0f,
			1.0f, 1.0f,

			0.0f, 0.0,
			1.0f, 1.0f,
			1.0f, 0.0f,

		};
		memcpy(texCoordData+(i*12), newTexData, sizeof(float) * 12);

		float newColorData[24] = {
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3],	
			 system->particles[i].color[0], system->particles[i].color[1], system->particles[i].color[2], system->particles[i].color[3]
		};
		memcpy(colorData+(i*24), newColorData, sizeof(float) * 24);

	}

	glVertexAttribPointer(positionAttribute, 3, GL_FLOAT, GL_FALSE, 0, vertexData);
	glEnableVertexAttribArray(positionAttribute);
	glVertexAttribPointer(texCoordAttribute, 2, GL_FLOAT, GL_FALSE, 0, texCoordData);
	glEnableVertexAttribArray(texCoordAttribute);
	glVertexAttribPointer(colorAttribute, 4, GL_FLOAT, GL_FALSE, 0, colorData);
	glEnableVertexAttribArray(colorAttribute);
	glBindTexture(GL_TEXTURE_2D, texture);
	glDrawArrays(GL_TRIANGLES, 0, system->particleCount * 6);
	
	glDisableVertexAttribArray(positionAttribute);
	glDisableVertexAttribArray(texCoordAttribute);
	glDisableVertexAttribArray(colorAttribute);
	
	free(vertexData);
	free(texCoordData);

}

#endif
