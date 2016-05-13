#include "perspective.hpp"
#include <iostream>
#include <math.h>

#define PI 3.141592653589793

TDPoint tdPoint(double x, double y, double z) {
	TDPoint point;
	point.x = x; point.y = y; point.z = z;
	return point;
}

void scalePoint(TDPoint *point, TDPoint scale) {
	point->x *= scale.x;
	point->y *= scale.y;
	point->z *= scale.z;
}

void translatePoint(TDPoint *point, TDPoint offset) {
	point->x += offset.x;
	point->y += offset.y;
	point->z += offset.z;
}

void rotateCartesianAroundAxis(TDPoint *point, double angle, int axis) {
	if(angle/360 == (int)(angle/360))
		return;
	Point plane;
	switch(axis) {
		case RotateXAxis:
			plane.x = point->z;
			plane.y = point->y;
			rotateCartesian(&plane, angle);
			point->z = plane.x;
			point->y = plane.y;
			break;
		case RotateYAxis:
			plane.x = point->x;
			plane.y = point->z;
			rotateCartesian(&plane, angle);
			point->x = plane.x;
			point->z = plane.y;
			break;
		case RotateZAxis:
			plane.x = point->x;
			plane.y = point->y;
			rotateCartesian(&plane, angle);
			point->x = plane.x;
			point->y = plane.y;
			break;
	}
}


void rotateCartesian(Point *point, double angle) {
	//rotate according to:
	//https://en.wikipedia.org/wiki/Rotation_matrix
	angle = angle/180*PI;
	double newX = point->x * cos(angle);
	newX += point->y * sin(angle);
	
	double newY = point->x * -sin(angle);
	newY += point->y * cos(angle);
	point->x = newX;
	point->y = newY;
}


void camTransform(TDPoint *point, CamParams cam) {
	//translate	
	translatePoint(point, tdPoint(-cam.x, -cam.y, -cam.z));
	rotateCartesianAroundAxis(point, -cam.yaw, RotateYAxis);
	rotateCartesianAroundAxis(point, -cam.pitch, RotateXAxis);
	rotateCartesianAroundAxis(point, -cam.roll, RotateZAxis);
}

Point projectPointToScreen(TDPoint point, CamParams cam, int width, int height) {
	
	camTransform(&point, cam);
	
	Point returnPoint;
	returnPoint.x = point.x/point.z;
	returnPoint.y = point.y/point.z;
	returnPoint.y *= -1;
	returnPoint.x /= tan(cam.fov/360*PI);
	returnPoint.y /= tan(cam.fov/360*PI); returnPoint.y *= (double)width/height;
	returnPoint.x *= (double)width/2; returnPoint.y *= (double)height/2;
	returnPoint.x += (double)width/2; returnPoint.y += (double)height/2;
	
	
	return returnPoint;
}
