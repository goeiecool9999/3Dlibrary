#ifndef OBJLOADING_H
#define OBJLOADING_H

#include <string>
#include <perspective.hpp>

struct Face {
	unsigned int *indices;
	unsigned int indicesSize;
};

struct Model {
	TDPoint *points;
	Face *faces;
	unsigned int pointsSize;
	unsigned int facesSize;
};

void loadObjFile(std::string path, Model& model);







#endif
