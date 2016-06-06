#ifndef PERSPECTIVE_H
#define PERSPECTIVE_H

#define RotateXAxis 0
#define RotateYAxis 1
#define RotateZAxis 2

struct TDPoint {
	double x,y,z;
};

struct Point {
	double x,y;
};

struct CamParams {
	double x,y,z;
	double yaw,pitch,roll;
	double fov;
};

void scalePoint(TDPoint *point, TDPoint scale);

void translatePoint(TDPoint *point, TDPoint offset);

void rotateCartesianAroundAxis(TDPoint *point, double angle, int axis);

void rotateCartesianWithOrigin(Point* point, Point origin, double angle);
void rotateCartesianWithOriginAroundAxis(TDPoint* point, TDPoint origin, double angle, int axis);

void rotateCartesian(Point *point, double angle);

TDPoint tdPoint(double x, double y, double z);
Point point(double x, double y);

Point projectPointToScreen(TDPoint point, CamParams cam, int width, int height);

#endif
