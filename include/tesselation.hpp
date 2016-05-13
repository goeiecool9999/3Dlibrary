#ifndef TESSELATION_H
#define TESSELATION_H

#define tesLines 		1
#define tesTrianles 	2
#define tesQuads 		3
#include <perspective.hpp>
#include "X11/Xlib.h"

struct TesselationState {
	unsigned int mode;
	bool strip;
	TDPoint *buffer;// array of size 4;
	int index;
};

struct DrawContext {
	Display *d;
	Drawable dest;
	int width,height;
	GC gc;
};

void prepareState(TesselationState *state);
void changeMode(int mode, TesselationState *state);

void tesVertex(double x, double y, double z, CamParams cam, TesselationState *state, DrawContext context);
void tesVertex(TDPoint point, CamParams cam, TesselationState *state, DrawContext context);

#endif
