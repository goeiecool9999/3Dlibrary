#include "objloading.hpp"
#include <iostream>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <fstream>

void getSizes(std::string& data, Model& model) {
	std::string read(data);
	model.pointsSize = 0;
	model.facesSize = 0;
	while (read.length() > 0) {
		std::string txtBeforeSpace = read.substr(0, read.find(' '));
		if(txtBeforeSpace == "v")
			model.pointsSize++;
		else if(txtBeforeSpace == "f")
			model.facesSize++;
		read = read.erase(0,read.find('\n')+1);
	}
}

void createLoadModelArrays(std::string& data, Model& model) {
	model.points = new TDPoint[model.pointsSize];
	model.faces = new Face[model.facesSize];
	std::string read(data);
	unsigned int pointIndex = 0,faceIndex = 0;
	while (read.length() > 0) {
		std::string txtBeforeSpace = read.substr(0, read.find(' '));
		if(txtBeforeSpace == "v" || txtBeforeSpace == "f") {
			std::string txtAfterSpace = read.substr(0, read.find('\n')).erase(0, read.find(' ')+1);
			
			if(txtBeforeSpace == "v") {
				char* ptr = strtok((char*)txtAfterSpace.c_str(), " ");
				double x = atof(ptr);
				ptr = strtok(NULL, " ");
				double y = atof(ptr);
				ptr = strtok(NULL, " ");
				double z = atof(ptr);
				model.points[pointIndex++] = tdPoint(x, y, z);
			} else if(txtBeforeSpace == "f") {
				//count the values on the line
				int values = 0;
				std::string scanSpaces(txtAfterSpace);
				while(scanSpaces.length() > 0) {
					size_t find = scanSpaces.find(' ');
					scanSpaces = scanSpaces.erase(0, find < scanSpaces.length() ? find+1 : find);
					values++;
				}
				//allocate memory for the indices
				values*=2;
				model.faces[faceIndex].indicesSize = values;
				model.faces[faceIndex].indices = new unsigned int[values];
				//create reference for easy access and increase index while we're at it
				unsigned int*& indices = model.faces[faceIndex++].indices;
				char* ptr = strtok((char*)txtAfterSpace.c_str(), " //");
				for(int i = 0; i < values; i++) {
					indices[i] = atoi(ptr);
					ptr = strtok (NULL, " //");
				}
			}
		}
		size_t find = read.find('\n');
		if(find+1 >= read.length())
			break;
		read = read.erase(0,find+1);
	}
}

void destroyModel(Model& model) {
	delete[] model.points;
	for (int i = 0; i < model.facesSize; i++)
		delete[] model.faces[i].indices;
	delete[] model.faces;
}

TDPoint modelAverage(Model& model) {
	
	
}


void loadObjFile(std::string path, Model& model) {
	std::fstream t(path.c_str());
	std::string str((std::istreambuf_iterator<char>(t)),
                 std::istreambuf_iterator<char>());
	getSizes(str, model);
	std::cout << "points: " << model.pointsSize << "\n";
	std::cout << "faces: " << model.facesSize << "\n";
	createLoadModelArrays(str, model);
}
