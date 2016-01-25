// chickpea unframework
// vector operations
// version 0.01
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#ifndef vector_h
#define vector_h

float vec3Dot(float v1[3], float v2[3]);
void vec3Reflect(float v1[3], float v2[3], float *out);

void vec3Multf(float v[3], float s, float *out);
void vec3Sub(float v1[3], float v2[3], float *out);

float vec2Dist(float v1[2], float v2[2]);
float vec2Length(float v[2]);

#endif

#ifdef CHICKPEA_VECTOR_IMPLEMENTATION

float vec3Dot(float v1[3], float v2[3]) {
	return v1[0] * v2[0] + v1[1] * v2[1] + v1[2] + v2[2];
}

void vec3Multf(float v[3], float s, float *out) {
	out[0] = v[0] * s;
	out[1] = v[1] * s;
	out[2] = v[2] * s;
}

void vec3Sub(float v1[3], float v2[3], float *out) {
	out[0] = v1[0] - v2[0];
	out[1] = v1[1] - v2[1];
	out[2] = v1[2] - v2[2];
}

void vec3Reflect(float v1[3], float v2[3], float *out) {
	float ret[3];
	vec3Multf(v2, 2.0 * vec3Dot(v2, v1), ret);
	vec3Sub(v1, ret, ret);
	memcpy(out, ret, sizeof(float) * 3);
}

float vec2Dist(float v1[2], float v2[2]) {
	float res[2] = {v1[0]-v2[0], v1[1]-v2[1]};
	return vec2Length(res);
}

float vec2Length(float v[2]) {
	return sqrtf(v[0]*v[0]+v[1]*v[1]);
}

#endif

