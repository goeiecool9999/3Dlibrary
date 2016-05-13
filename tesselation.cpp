#include "tesselation.hpp"
#include "X11/Xlib.h"
#include <iostream>

void prepareState(TesselationState *state) {
	state->buffer = new TDPoint[4];
	state->index = 0;
	state->mode = 0;
	state->strip = false;
}

void changeMode(int mode, TesselationState *state) {
	state->mode = mode;
	state->index = 0;
}

void drawBufferContent(CamParams cam, TesselationState *state, DrawContext context) {
	Point screenPoints[state->mode + 1];
	for(int i = 0; i <= state->mode; i++) {
		screenPoints[i] = projectPointToScreen(state->buffer[i], cam, context.width, context.height);
	}
	
	
	if(state->mode == tesLines) {
		XDrawLine(context.d, context.dest, context.gc, screenPoints[0].x, screenPoints[0].y, screenPoints[1].x, screenPoints[1].y);
	} else if(state->mode == tesTrianles) {
		XDrawLine(context.d, context.dest, context.gc, screenPoints[0].x, screenPoints[0].y, screenPoints[1].x, screenPoints[1].y);
		XDrawLine(context.d, context.dest, context.gc, screenPoints[1].x, screenPoints[1].y, screenPoints[2].x, screenPoints[2].y);
		XDrawLine(context.d, context.dest, context.gc, screenPoints[0].x, screenPoints[0].y, screenPoints[2].x, screenPoints[2].y);
	} else if(state->mode == tesQuads) {
		XDrawLine(context.d, context.dest, context.gc, screenPoints[0].x, screenPoints[0].y, screenPoints[1].x, screenPoints[1].y);
		XDrawLine(context.d, context.dest, context.gc, screenPoints[1].x, screenPoints[1].y, screenPoints[2].x, screenPoints[2].y);
		XDrawLine(context.d, context.dest, context.gc, screenPoints[2].x, screenPoints[2].y, screenPoints[3].x, screenPoints[3].y);
		XDrawLine(context.d, context.dest, context.gc, screenPoints[3].x, screenPoints[3].y, screenPoints[0].x, screenPoints[0].y);
	}
}

void tesVertex(TDPoint point, CamParams cam, TesselationState *state, DrawContext context) {
	state->buffer[state->index] = point;
	if(state->index < state->mode) {
		state->index++;
		return;
	}
	state->index = 0;
	drawBufferContent(cam, state, context);
	
	if(state->strip && state->mode == tesLines) {
		state->buffer[state->index] = state->buffer[state->index+1];
		state->index = 1;
	}
}

void tesVertex(double x, double y, double z, CamParams cam, TesselationState *state, DrawContext context) {
	tesVertex(tdPoint(x,y,z), cam, state, context);
}
