#ifndef RASTERIZE_H
#define RASTERIZE_H

#include "tesselation.hpp"
#include "image.hpp"

void sortTriangleByX(int *indices, Point *points);
void fillTriangle(DrawContext context, Point* points, unsigned int triangles);
void fillTriangle(DrawContext context, Point* points, Point* texturePoints, unsigned int triangles, Image image);

#endif
