#include "rasterize.hpp"
#include "tesselation.hpp"
#include "image.hpp"
#include <math.h>
#include <X11/Xlib.h>
#include <iostream>

#define PI 3.141592653589793

void rotateTriangleAndGetAngle(Point* triangle, Point *point, double *angle) {	
	double dy = triangle[1].y - triangle[0].y;
	double dx = triangle[1].x - triangle[0].x;
	
	double sourceTAngle = atan2(dy,dx);
	
	{
		Point origin = triangle[0];
		bool additionalRotation = false;
		for(int i = 0; i < 3; i++) {
			triangle[i].x -= origin.x;
			triangle[i].y -= origin.y;
			rotateCartesian(&triangle[i], sourceTAngle/PI*180);
		}
		if(point) {
			point->x -= origin.x;
			point->y -= origin.y;
			rotateCartesian(point, sourceTAngle/PI*180);
		}
		if(triangle[2].y < 0)
				additionalRotation = true;
		for(int i = 0; i < 3; i++) {
			triangle[i].x += origin.x;
			triangle[i].y += origin.y;
		}
		if(point) {
			point->x += origin.x;
			point->y += origin.y;
		}
				
		if(additionalRotation) {
			sourceTAngle += PI;
			for(int i = 0; i < 3; i++)
				rotateCartesianWithOrigin(&triangle[i], origin, 180);//rotate the additional 180 degrees.
			if(point) {
				rotateCartesianWithOrigin(point, origin, 180);//rotate the additional 180 degrees.
			}
		}
	}
	while (sourceTAngle > 180) {
		sourceTAngle -= 2*PI; // wtf am I even doing at this point.... Effinciency down the drain.
	}
	while (sourceTAngle < -180) {
		sourceTAngle += 2*PI;
	}
	*angle = sourceTAngle;
}


/*done extremeley poorly should be done completely differently but
 * this is just a quick implementation to make it work
 * In the future it should be changed with proper mapping math.
 * No calling sin cos and atan2 several times for abysmal performance.*/
void mapPointToTexture(Point *p, Point *sourceT, Point *destT) {
	/* So basically the plan here was to avoid all the complex math
	 * involving matrices. And just use dodgy shearing.*/
	
	Point refT[3];
	for(int i = 0; i < 3; i++) {
		refT[i] = sourceT[i];
	}
	
	double sourceTAngle;
	rotateTriangleAndGetAngle(refT, p, &sourceTAngle);
	
	Point destRot[3];
	for(int i = 0; i < 3; i++)
		destRot[i] = destT[i];
	double destTAngle;
	rotateTriangleAndGetAngle(destRot, NULL, &destTAngle);
		
	//Make origin of refT the pivot.
	{
		Point temp = refT[0];
		for(int i = 0; i < 3; i++) {
			refT[i].x -= temp.x;
			refT[i].y -= temp.y;
		}
		p->x -= temp.x;
		p->y -= temp.y;
	}
	
	
	//first we make sure that the base (meaning the horizontal one)
	//are the same length from the pivot in the same direction.
	//Also at the same time we adjust the triangle height.
	
	{
		//width adjustment
		double ratio = (destRot[1].x-destRot[0].x)/(refT[1].x-refT[0].x);
		//stretch with pivot as X axis
		for(int i = 0; i < 3; i++) {
			refT[i].x *= ratio;
		}
		p->x *= ratio;
		//height adjustment
		ratio = (destRot[2].y-destRot[0].y)/(refT[2].y-refT[0].y);
		//stretch with pivot as Y axis
		for(int i = 0; i < 3; i++) {
			refT[i].y *= ratio;
		}
		p->y *= ratio;
	}
	
	//now it's time to skew refT to match destT.
	{
		//how far we have to move.
		double offset = (refT[2].x-refT[0].x)-(destRot[2].x-destRot[0].x);
		p->x -= p->y/refT[2].y*offset;
		refT[2].x -= offset;
	}
	
	for(int i = 0; i < 3; i++) {
		rotateCartesian(&refT[i], -destTAngle/PI*180);
	}
	rotateCartesian(p, -destTAngle/PI*180);
	
	
	//Match the pivots
	{
		for(int i = 0; i < 3; i++) {
			refT[i].x += destT[0].x;
			refT[i].y += destT[0].y;
		}
		p->x += destT[0].x;
		p->y += destT[0].y;
	}	
}

void sortTriangleByX(int *indices, Point *points) {
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
	indices[0] = left;
	indices[1] = middle;
	indices[2] = right;
}

void fillTriangle(DrawContext context, Point* points, unsigned int triangles) {
	Image img;
	img.width = 0; img.height = 0;
	fillTriangle(context, points, NULL, triangles, img);
}

//texture coords are from 0-1 (should I be typing this stuff in the header files?)

void fillTriangle(DrawContext context, Point* points, Point* texturePoints, unsigned int triangles, Image image) {
	
	//iterate over all triangles in the points list.
	for(unsigned int i = 0; i < triangles; i++) {
		//find the points for the triangle we're working on and store it in the temp array points.
		unsigned int index = i*3;
		Point* oldPoints = points;
		Point points[3] = {oldPoints[index], oldPoints[index+1], oldPoints[index+2]};
		bool texture = (image.width >= 0 && image.height >= 0);
		Point* oldTPoints;
		Point tPoints[3];
		if(texture) {
			oldTPoints = texturePoints;
			for(int i = 0; i < 3; i++) {
				tPoints[i] = oldTPoints[index+i];
			}
		}
		
		int indices[3] = {0,0,0};
		sortTriangleByX(indices, points);
		int left = indices[0];
		int middle = indices[1];
		int right = indices[2];
		
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
			for(int y = y1; y <= y2; y++) {
				if(texture) {
					Point p = point(x,y);
					mapPointToTexture(&p, points, tPoints);
					unsigned long color = getColorFromPixel((int)(p.x*image.width), (int)(p.y*image.height), image);
					XSetForeground(context.d, context.gc, color);
				}
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
				if(texture) {
					Point p = point(x,y);
					mapPointToTexture(&p, points, tPoints);
					unsigned long color = getColorFromPixel((int)(p.x*image.width), (int)(p.y*image.height), image);
					XSetForeground(context.d, context.gc, color);
				}
				XDrawPoint(context.d, context.dest, context.gc, x, y);
			}
		}



		
	}
}
