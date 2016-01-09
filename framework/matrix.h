// chickpea unframework
// ivan safrin, 2016
// this code is licensed under the MIT license

#ifndef matrix_h
#define matrix_h

#include <string.h>
#include <math.h>

void matrixMultiply(float *m, float *m2, float *target);
void matrixSetIdentity(float *m);
void matrixSetOrthoProjection(float *m, float left, float right, float bottom, float top, float zNear, float zFar);
void matrixSetRoll(float *m, float roll);
void matrixRoll(float *m, float roll);
void matrixSetPosition(float *m, float x, float y, float z);
void matrixTranslate(float *m, float x, float y, float z);
void matrixSetScale(float *m, float x, float y, float z);
void matrixScale(float *m, float x, float y, float z);
void matrixSetPitch(float *m, float pitch);
void matrixPitch(float *m, float pitch);
void matrixSetYaw(float *m, float yaw);
void matrixYaw(float *m, float yaw);


void matrixMultiply(float *m, float *m2, float *target) {
	float mout[16];
	
	mout[0] = m[0] * m2[0] + m[1] * m2[4] + m[2] * m2[8] + m[3] * m2[12];
	mout[1] = m[0] * m2[1] + m[1] * m2[5] + m[2] * m2[9] + m[3] * m2[13];
	mout[2] = m[0] * m2[2] + m[1] * m2[6] + m[2] * m2[10] + m[3] * m2[14];
	mout[3] = m[0] * m2[3] + m[1] * m2[7] + m[2] * m2[11] + m[3] * m2[15];

	mout[4] = m[4] * m2[0] + m[5] * m2[4] + m[6] * m2[8] + m[7] * m2[12];
	mout[5] = m[4] * m2[1] + m[5] * m2[5] + m[6] * m2[9] + m[7] * m2[13];
	mout[6] = m[4] * m2[2] + m[5] * m2[6] + m[6] * m2[10] + m[7] * m2[14];
	mout[7] = m[4] * m2[3] + m[5] * m2[7] + m[6] * m2[11] + m[7] * m2[15];

	mout[8] = m[8] * m2[0] + m[9] * m2[4] + m[10] * m2[8] + m[11] * m2[12];
	mout[9] = m[8] * m2[1] + m[9] * m2[5] + m[10] * m2[9] + m[11] * m2[13];
	mout[10] = m[8] * m2[2] + m[9] * m2[6] + m[10] * m2[10] + m[11] * m2[14];
	mout[11] = m[8] * m2[3] + m[9] * m2[7] + m[10] * m2[11] + m[11] * m2[15];

	mout[12] = m[12] * m2[0] + m[13] * m2[4] + m[14] * m2[8] + m[15] * m2[12];
	mout[13] = m[12] * m2[1] + m[13] * m2[5] + m[14] * m2[9] + m[15] * m2[13];
	mout[14] = m[12] * m2[2] + m[13] * m2[6] + m[14] * m2[10] + m[15] * m2[14];
	mout[15] = m[12] * m2[3] + m[13] * m2[7] + m[14] * m2[11] + m[15] * m2[15];
	
	memcpy(target, mout, sizeof(float) * 16);
}

void matrixSetIdentity(float *m) {
	m[0] = 1.0;
	m[1] = 0.0;
	m[2] = 0.0;
	m[3] = 0.0;
	
	m[4] = 0.0;
	m[5] = 1.0;
	m[6] = 0.0;
	m[7] = 0.0;
	
	m[8] = 0.0;
	m[9] = 0.0;
	m[10] = 1.0;
	m[11] = 0.0;
	
	m[12] = 0.0;
	m[13] = 0.0;
	m[14] = 0.0;
	m[15] = 1.0;
}

void matrixSetOrthoProjection(float *m, float left, float right, float bottom, float top, float zNear, float zFar) {
	m[0] = 2.0f/(right-left);
	m[5] = 2.0f/(top-bottom);
	m[10] = -2.0f/(zFar-zNear);
	
	m[12] = -((right+left)/(right-left));
	m[13] = -((top+bottom)/(top-bottom));
	m[14] = -((zFar+zNear)/(zFar-zNear));
}


void matrixSetRoll(float *m, float roll) {
	m[0] = cos(roll);
	m[4] = -sin(roll);
	m[1] = sin(roll);
	m[5] = cos(roll);
}

void matrixRoll(float *m, float roll) {
	float m2[16];
	matrixSetIdentity(m2);
	matrixSetRoll(m2, roll);
	matrixMultiply(m2, m, m);
}

void matrixSetPosition(float *m, float x, float y, float z) {
	m[12] = x;
	m[13] = y;
	m[14] = z;
}

void matrixTranslate(float *m, float x, float y, float z) {
	float m2[16];
	matrixSetIdentity(m2);
	matrixSetPosition(m2, x, y, z);
	matrixMultiply(m2, m, m);
}

void matrixSetScale(float *m, float x, float y, float z) {
	m[0] = x;
	m[5] = y;
	m[10] = z;
}

void matrixScale(float *m, float x, float y, float z) {
	float m2[16];
	matrixSetIdentity(m2);
	matrixSetScale(m2, x, y, z);
	matrixMultiply(m2, m, m);
}

void matrixSetPitch(float *m, float pitch) {
	m[5] = cos(pitch);
	m[9] = -sin(pitch);
	m[6] = sin(pitch);
	m[10] = cos(pitch);
}

void matrixPitch(float *m, float pitch) {
	float m2[16];
	matrixSetIdentity(m2);
	matrixSetPitch(m2, pitch);
	matrixMultiply(m2, m, m);
}

void matrixSetYaw(float *m, float yaw) {
	m[0] = cos(yaw);
	m[8] = sin(yaw);
	m[2] = -sin(yaw);
	m[10] = cos(yaw);
}


void matrixYaw(float *m, float yaw) {
	float m2[16];
	matrixSetIdentity(m2);
	matrixSetYaw(m2, yaw);
	matrixMultiply(m2, m, m);
}

#endif
