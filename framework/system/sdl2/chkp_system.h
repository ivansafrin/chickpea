// chickpea unframework
// OpenGL ES2 system utilities (SDL2)
//
// This software is in the public domain. Where that dedication is not
// recognized, you are granted a perpetual, irrevocable license to copy
// and modify it however you want.
//
// People who worked on this file:
//	Ivan Safrin

#include <SDL.h>
#include <SDL_opengl.h>

#ifndef CHICKPEA_SYSTEM_H
#define CHICKPEA_SYSTEM_H

typedef struct {
	SDL_Window* displayWindow;
	SDL_GLContext context;
	SDL_Event event;
} ChickpeaWindow;

typedef enum {EVENT_NONE=0, EVENT_QUIT} SystemEventType;

typedef struct {
	SystemEventType type;		
} SystemEvent;

int systemInit(ChickpeaWindow* window, const char *title, uint32_t xRes, uint32_t yRes, uint8_t fullscreen);
void systemShowFrame(ChickpeaWindow* window);
void systemShutdown();
uint8_t systemPollEvent(ChickpeaWindow* window, SystemEvent *event);

#endif

#ifdef CHICKPEA_SYSTEM_IMPLEMENTATION

void systemShowFrame(ChickpeaWindow* window) {
	SDL_GL_SwapWindow(window->displayWindow);
}

void systemShutdown() {
	SDL_Quit();
}

uint8_t systemPollEvent(ChickpeaWindow* window, SystemEvent *event) {
	event->type = EVENT_NONE;
	uint8_t ret = SDL_PollEvent(&window->event);
	if(!ret) {
		return 0;
	}
	switch(window->event.type) {
		case SDL_QUIT:
		case SDL_WINDOWEVENT_CLOSE:
			event->type = EVENT_QUIT;
		break;
	}
	return 1;
}

int systemInit(ChickpeaWindow *window, const char *title, uint32_t xRes, uint32_t yRes, uint8_t fullscreen) {
	SDL_Init(SDL_INIT_VIDEO|SDL_INIT_AUDIO);
	uint32_t flags = SDL_WINDOW_OPENGL;
	if(fullscreen) {
		flags |= SDL_WINDOW_FULLSCREEN;
	}
	window->displayWindow = SDL_CreateWindow(title, SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, xRes, yRes, flags);
	window->context = SDL_GL_CreateContext(window->displayWindow);
	SDL_GL_MakeCurrent(window->displayWindow, window->context);
	glViewport(0, 0, xRes, yRes);
	return 1;
}



#endif
