/*
 Scene built from cubes, squares and a sphere
*/

#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <sstream>
#include <string>
#include <math.h>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "Viewer.h"
#include "Objects.h"
#include "Shader.hpp"

using namespace std;

// Variables controlling how the scene is drawn
Objects *TheObjects;

int winX = 500;
int winY = 500;

// float xRot = 0;
float yRot = 0;
float trans = 0;

// Cameras
ObjectViewer *ObjCam;
Viewer *Camera;

unsigned int programID;

bool lPressed = false;

// check if location valid
bool valid(glm::vec3 location)
{
	int xpos = round(TheObjects->size + location[0]) / 2;
	int ypos = round(TheObjects->size + location[2]) / 2;
	if (xpos == TheObjects->size)
		xpos = TheObjects->size - 1;
	else if (xpos > TheObjects->size)
		return false;
	else if (xpos < 0)
		return false;
	else if (ypos < 0)
		return false;
	else if (ypos > TheObjects->size - 1)
		return false;
	if (TheObjects->map[ypos][xpos] == '*')
		return false;
	return true;
}

// 
bool goalReached(glm::vec3 location)
{
	int xpos = round(TheObjects->size + location[0]) / 2;
	int ypos = round(TheObjects->size + location[2]) / 2;
	if (TheObjects->map[ypos][xpos] == 'X')
		return true;
	return false;
}

// Callbacks
void key_callback(GLFWwindow* window,
				  int key, int scancode, int action, int mods)
{
	if (action == GLFW_PRESS)
	{
		switch(key) 
		{
		case GLFW_KEY_T: // toggle which texture
			TheObjects->ToggleTexture();
			break;
		case GLFW_KEY_ESCAPE: // escape key pressed
			glfwSetWindowShouldClose(window, GL_TRUE);
			break;
		case GLFW_KEY_L:
		{
			lPressed = !lPressed;
			GLenum glMode = lPressed ? GL_LINE : GL_FILL;
			glPolygonMode(GL_FRONT_AND_BACK, glMode);
			break;
		}
		case GLFW_KEY_UP:
			if(valid(Camera->cameraPos + Camera->front))
			{
				Camera->cameraPos += Camera->front;
				if (goalReached(Camera->cameraPos))
				{
					Camera->reset();
				}
				else
					trans = 1;
			}
			break;
		case GLFW_KEY_DOWN:
			if(valid(Camera->cameraPos - Camera->front))
			{
				Camera->cameraPos -= Camera->front;
				if (goalReached(Camera->cameraPos))
				{
					Camera->reset();
				}
				else
					trans = -1;
			}
			break;
		default:
			break;
		}
	}
	else 
	{
		yRot = 0;
	}

	if (action == GLFW_PRESS || action == GLFW_REPEAT)
	{
		switch(key) 
		{
		case GLFW_KEY_LEFT:
			yRot = -0.02;
			break;
		case GLFW_KEY_RIGHT:
			yRot = 0.02;
			break;
		default:
			break;
		}
	}
	else 
	{
		trans = 0;
	}
}	

// Set the projection matrix. Takes into account window aspect ratio, so called
// when the window is resized.
void setProjection()
{
	glm::mat4 projection;
	
	projection = glm::perspective( (float)M_PI/3.0f, (float) winX / winY, 1.0f, 30.0f );

	// Load it to the shader program
	int projHandle = glGetUniformLocation(programID, "projection");
	if (projHandle == -1) {
		std::cout << "Uniform: projection_matrix is not an active uniform label\n";
	}
	glUniformMatrix4fv( projHandle, 1, false, glm::value_ptr(projection) );
}    

// Called when the window is resized.
void reshape_callback( GLFWwindow *window, int x, int y ) 
{
	winX = x;
	winY = y;
	setProjection();
	glViewport( 0, 0, x, y );
}

void render() 
{
	// Update the camera, and draw the scene.
	glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

	Camera->update( yRot, trans );    

	// First load the viewing matrix from the current camera
	glm::mat4 viewMatrix;
	viewMatrix = Camera->getViewMtx();
	
	// Load it to the shader program
	int viewHandleCube = glGetUniformLocation(programID, "view");
	if (viewHandleCube == -1) {
		std::cout << "Uniform: view is not an active uniform label\n";
	}
	glUniformMatrix4fv( viewHandleCube, 1, false, glm::value_ptr(viewMatrix) );

	TheObjects->render();
}

// Error callback for GLFW. Simply prints error message to stderr
static void error_callback(int error, const char* description)
{
	fputs(description, stderr);
}

int main (int argc, char **argv)
{
	GLFWwindow* window;
	glfwSetErrorCallback(error_callback);
	
	if (!glfwInit()) {
		exit(1);
	}

	// Specify that we want OpenGL 3.3
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	// Create the window and OpenGL context
	window = glfwCreateWindow(winX, winY, "Modelling and viewing", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		exit(1);
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);
	
	// Initialize GLEW
	glewExperimental = true; // Needed for core profile
	if (glewInit() != GLEW_OK) {
		fprintf(stderr, "Failed to initialize GLEW\n");
		exit(1);
	}

	// Set up the shaders we are to use. 0 indicates error.
	programID = LoadShaders("cube.vert", "cube.frag");
	if (programID == 0) {
		exit(1);
	}

	// Set OpenGL state we need for this application.
	glClearColor(0.5F, 0.5F, 0.5F, 0.0F);
	glEnable(GL_DEPTH_TEST);
	glUseProgram(programID);
	
	// Set up the scene and the cameras
	setProjection();
	
	TheObjects = new Objects(programID);
	TheObjects->readMap(argv[1]);

	ObjCam = new ObjectViewer(TheObjects->size);
	Camera = ObjCam;

	// Define callback functions and start main loop
	glfwSetKeyCallback(window, key_callback);
	glfwSetFramebufferSizeCallback(window, reshape_callback);

	while (!glfwWindowShouldClose(window))
	{
		render();

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	// Clean up
	delete TheObjects;
	
	glfwDestroyWindow(window);
	glfwTerminate();
	exit(0);
	
	return 0;
}
