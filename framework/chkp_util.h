// chickpea unframework
// general useful utility functions
// version 0.01
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#ifndef util_h
#define util_h

#define PI 3.1415926536
#define RANDOM_FLOAT ((float)rand()/(float)RAND_MAX)

float lerp(float v0, float v1, float t);
float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax);

float easeIn(float from, float to, float time);
float easeOut(float from, float to, float time);
float easeInOut(float from, float to, float time);
float easeOutElastic(float from, float to, float time);

#endif

#ifdef CHICKPEA_UTIL_IMPLEMENTATION

float lerp(float v0, float v1, float t) {
	return (1.0-t)*v0 + t*v1;
}

float mapValue(float value, float srcMin, float srcMax, float dstMin, float dstMax) {
	float retVal = dstMin + ((value - srcMin)/(srcMax-srcMin) * (dstMax-dstMin));
	if(retVal < dstMin) {
		retVal = dstMin;
	}
	if(retVal > dstMax) {
		retVal = dstMax;
	}
	return retVal;
}

float easeIn(float from, float to, float time) {
	float tVal = time*time*time*time*time;
	return (1.0f-tVal)*from + tVal*to;
}

float easeOut(float from, float to, float time) {
	float oneMinusT = 1.0f-time;
	float tVal =  1.0f - (oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT);
	return (1.0f-tVal)*from + tVal*to;
}

float easeInOut(float from, float to, float time) {
	float tVal;
	if(time > 0.5) {
		float oneMinusT = 1.0f-((0.5f-time)*-2.0f);
		tVal =  1.0f - ((oneMinusT * oneMinusT * oneMinusT * oneMinusT * oneMinusT) * 0.5f);
	} else {
		time *= 2.0;
		tVal = (time*time*time*time*time)/2.0;
	}
	return (1.0f-tVal)*from + tVal*to;
}

float easeOutElastic(float from, float to, float time) {
	float p = 0.3f;
	float s = p/4.0f;
	float diff = (to - from);
	return from + diff + (diff*pow(2.0f,-10.0f*time) * sin((time-s)*(2*PI)/p));
}

#endif
