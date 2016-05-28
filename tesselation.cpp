#include "tesselation.hpp"
#include "X11/Xlib.h"
#include <iostream>

void prepareState(TesselationState *state) {
	state->buffer = new Point[4];
	state->index = 0;
	state->mode = 0;
	state->strip = false;
}

void changeMode(int mode, TesselationState *state) {
	state->mode = mode;
	state->index = 0;
}

void drawBufferContent(TesselationState *state, DrawContext context) {
	if(state->mode == tesLines) {
		XDrawLine(context.d, context.dest, context.gc, state->buffer[0].x, state->buffer[0].y, state->buffer[1].x, state->buffer[1].y);
	} else if(state->mode == tesTriangles) {
		XDrawLine(context.d, context.dest, context.gc, state->buffer[0].x, state->buffer[0].y, state->buffer[1].x, state->buffer[1].y);
		XDrawLine(context.d, context.dest, context.gc, state->buffer[1].x, state->buffer[1].y, state->buffer[2].x, state->buffer[2].y);
		XDrawLine(context.d, context.dest, context.gc, state->buffer[0].x, state->buffer[0].y, state->buffer[2].x, state->buffer[2].y);
	} else if(state->mode == tesQuads) {
		XDrawLine(context.d, context.dest, context.gc, state->buffer[0].x, state->buffer[0].y, state->buffer[1].x, state->buffer[1].y);
		XDrawLine(context.d, context.dest, context.gc, state->buffer[1].x, state->buffer[1].y, state->buffer[2].x, state->buffer[2].y);
		XDrawLine(context.d, context.dest, context.gc, state->buffer[2].x, state->buffer[2].y, state->buffer[3].x, state->buffer[3].y);
		XDrawLine(context.d, context.dest, context.gc, state->buffer[3].x, state->buffer[3].y, state->buffer[0].x, state->buffer[0].y);
	}
}

inline void checkBufferSize(TesselationState *state, DrawContext context) {
	if(state->index < state->mode) {
		state->index++;
		return;
	}
	state->index = 0;
	drawBufferContent(state, context);
	
	if(state->strip && state->mode == tesLines) {
		state->buffer[state->index] = state->buffer[state->index+1];
		state->index = 1;
	}
}

void tesTDVertex(TDPoint point, CamParams cam, TesselationState *state, DrawContext context) {
	state->buffer[state->index] = projectPointToScreen(point, cam, context.width, context.height);
	checkBufferSize(state, context);
}

void tesTDVertex(double x, double y, double z, CamParams cam, TesselationState *state, DrawContext context) {
	tesTDVertex(tdPoint(x,y,z), cam, state, context);
}

Point scalePoint(Point point, int xMin, int xMax, int yMin, int yMax, int sWidth, int sHeight) {
	Point sPoint = point;
	sPoint.x -= xMin;
	sPoint.y -= yMin;
	sPoint.x *= (float)sWidth/(xMax-xMin);
	sPoint.y *= (float)sHeight/((int)yMax-(int)yMin);
	return sPoint;
}

void tesVertex(Point point, DrawContext context, TesselationState *state, int xMin, int xMax, int yMin, int yMax) {
	state->buffer[state->index] = scalePoint(point, xMin, xMax, yMin, yMax, context.width, context.height);
	checkBufferSize(state, context);
}

void tesVertex(double x, double y, DrawContext context, TesselationState *state, int xMin, int xMax, int yMin, int yMax) {
	tesVertex(point(x,y), context, state, xMin, xMax, yMin, yMax);
}
