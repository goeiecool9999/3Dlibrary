#ifndef RASTERIZE_H
#define RASTERIZE_H

#include "tesselation.hpp"

void sortTriangleByX(int *indices, Point *points);
void fillTriangle(DrawContext context, Point* points, unsigned int triangles);

#endif
