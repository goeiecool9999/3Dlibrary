#include "rasterize.hpp"
#include "tesselation.hpp"
#include <X11/Xlib.h>
#include <iostream>

void fillTriangle(DrawContext context, Point* points, unsigned int triangles) {
	
	//iterate over all triangles in the points list.
	for(unsigned int i = 0; i < triangles; i++) {
		//find the points for the triangle we're working on and store it in the temp array points.
		unsigned int index = i*3;
		Point* oldPoints = points;
		Point points[3] = {oldPoints[index], oldPoints[index+1], oldPoints[index+2]};
		
		//sort points from left to right.
		int left=0,middle=1,right=2;
		//find points left to right.
		//compare first and second points.
		bool greater = points[0].x >= points[1].x;
		//store largest in right and smallest in middle.
		middle = greater ? 1 : 0;
		right = !greater ? 1 : 0;
		//compare third point to current middle point.
		if(points[2].x >= points[middle].x) {
			//if it is greater, store it in middle and move current middle to left
			left = middle;
			middle = 2;
			//since its bigger than the last middle it might also be bigger than the current right.
			//Check for that and correct accordingly.
			if(points[2].x >= points[right].x) {
				middle = right;
				right = 2;
			}
		} else {
			left = 2;
		}
		
		//For this next explanation the leftmost point will be refered to as L.
		//the rightmost point as R and the middle point as T.
		//the points are connected by lines LR, LT and RT, creating triangle LTR
		
		//We can split triangle LTR up into two by splitting it through the vertical line X = Tx (Tx means x of point T)
		//Giving us 2 triangles that both have a vertical line. We'll call them triangles LTM and MTR
		//where M is the new point that came from splitting LTR.
		
		//We can now fill LTM and MTR by drawing vertical lines of the appropriate
		//color in between their non-vertical lines.
		//First we draw vertical lines between LT and LM and then between RT and MR.
		
		const double lx = points[left].x;
		const double mx = points[middle].x;
		const double rx = points[right].x;
		const double ly = points[left].y;
		const double my = points[middle].y;
		const double ry = points[right].y;
		const double by = (mx-lx)/(rx-lx)*(ry-ly)+ly;
		
		if(by == my) {
			XDrawLine(context.d, context.dest, context.gc, lx, ly, rx, ry);
			continue;
		}
		
		
		for(int x = lx; x < mx; x++) {
			//LT
			int y1 = (x-lx)/(mx-lx)*(my-ly)+ly;
			int y2 = (x-lx)/(rx-lx)*(ry-ly)+ly;
			if (y1>=y2) {
				int tmp = y2;
				y2 = y1;
				y1 = tmp;
			}
			for(int y = y1; y <= y2; y++){
				XDrawPoint(context.d, context.dest, context.gc, x, y);
			}
		}
		
		for(int x = mx; x <= rx; x++) {
			//RT
			int y1 = (x-mx)/(rx-mx)*(ry-my)+my;
			int y2 = (x-lx)/(rx-lx)*(ry-ly)+ly;
			if (y1>=y2) {
				int tmp = y2;
				y2 = y1;
				y1 = tmp;
			}
			for(int y = y1; y <= y2; y++){
				XDrawPoint(context.d, context.dest, context.gc, x, y);
			}
		}



		
	}
}
