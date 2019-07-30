
#include <iostream>
using namespace std;

#include "Viewer.h"
#include "glm/gtc/matrix_transform.hpp"

#define DEG2RAD(x) ((x)*M_PI/180.0) 
#define RAD2DEG(x) ((x)*180.0/M_PI) 

Viewer::Viewer( glm::vec3 eye, int dimension )
{
	size = dimension;
	initEye = eye;
	front = glm::vec3(1.0, 0.0, 0.0);
	reset();
}

Viewer::~Viewer(){}

/**
 Assumes the current matrix has been calculated (usually in 
 update() or reset())
*/
const glm::mat4 Viewer::getViewMtx() const
{
	return viewMtx;
}

/**
 Resets the view matrix to the value the camera was 
 initialised with. 
*/
void Viewer::reset()
{
	cameraPos = glm::vec3(-size, 0.0f, -size);
	glm::vec3 at(0.0f, 0.0f, -5.0f);
	glm::vec3 up(0.0f, 1.0f, 0.0f);
	viewMtx = glm::lookAt(cameraPos, at, up);
}

ObjectViewer::ObjectViewer(int dimension)
	: Viewer(cameraPos, dimension)
{}

void ObjectViewer::update( float y, float trans ) 
{
	if (y != 0)
	{
		viewMtx = glm::translate(viewMtx, cameraPos);
		viewMtx = glm::rotate(viewMtx, y, glm::vec3(0.0f, 1.0f, 0.0f));
		viewMtx = glm::translate(viewMtx, -cameraPos);
		front = -glm::vec3(viewMtx[0][2], viewMtx[1][2], viewMtx[2][2]);
	}

	if (trans != 0)
	{
		viewMtx = glm::lookAt(cameraPos, cameraPos + front, glm::vec3(0.0, 1.0, 0.0));
	}

}