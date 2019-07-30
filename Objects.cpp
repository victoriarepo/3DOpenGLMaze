#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include "Objects.h"
#include "Sphere.hpp"

#include <GL/glew.h>

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtc/type_ptr.hpp"

using namespace std;

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define NUM_TEXTURES 3

#define TEX_SIZE 64  // resolution of our constructed texture image

#define CUBE_NUM_VERTICES 8
#define CUBE_NUM_TRIS 12
#define VALS_PER_VERT 3
#define VALS_PER_NORMAL 3
#define VALS_PER_TEX 2          // number of dimensions of the texture data (if image, set it to 2)
#define POLYGON_NUM_TRIS 2      // number of triangles in our mesh
#define POLYGON_NUM_VERTICES 8     // number of vertices in our mesh

// texture image data
GLubyte myimage[TEX_SIZE][TEX_SIZE][3];

/*
 Variables defining texture settings
*/
static GLenum CurrentTexSample = GL_NEAREST;
static char   CurrentTexString[32] = "GL_NEAREST";

static GLenum CurrentTexMode = GL_REPLACE;
static char   CurrentTexModeString[32] = "GL_REPLACE";

GLuint TexID[NUM_TEXTURES];

unsigned int vaoHandleCube, vaoHandleFloor, vaoHandleGoal;
Sphere *sphere;
glm::vec3 colour = glm::vec3(0.0, 0.0, 0.0);

// toggling texture on and off
void Objects::ToggleTexture()
{
	textureEnabled = !textureEnabled;
}

// Creates a new vertex array object for a cube and loads in data into a vertex attribute buffer
int createCubeVAO(int programID)
{
	// Cube vertex data
	float Vertices[ 3 * CUBE_NUM_VERTICES * VALS_PER_VERT ] = {
		-1.0, -1.0, 1.0,
		1.0, -1.0,  1.0,
		1.0,  1.0,  1.0,
		-1.0,  1.0,  1.0,

		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0,  1.0, -1.0,
		-1.0,  1.0, -1.0,

		-1.0, -1.0, -1.0,
		-1.0, -1.0, 1.0,
		-1.0,  1.0, -1.0,
		-1.0,  1.0, 1.0,

		1.0, -1.0, -1.0,
		1.0, -1.0, 1.0,
		1.0,  1.0, -1.0,
		1.0,  1.0, 1.0,

		-1.0, 1.0, -1.0,
		-1.0, 1.0, 1.0,
		1.0,  1.0, -1.0,
		1.0,  1.0, 1.0,

		-1.0, -1.0, -1.0,
		-1.0, -1.0, 1.0,
		1.0,  -1.0, -1.0,
		1.0,  -1.0, 1.0
	};

	// Texture coords for each vertex. 2 per vertex.
	float tex_coord[3 * POLYGON_NUM_VERTICES * VALS_PER_TEX] = {
		0.0f, 0.0f,
		1.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		0.0f, 0.0f,
		1.0f, 0.0f,
		0.0f, 1.0f,
		1.0f, 1.0f,

		1.0f, 0.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		0.0f, 1.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,

		0.0f, 1.0f,
		0.0f, 0.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
	};

	// 12 triangles - 2 per face of the cube
	unsigned int Indices[CUBE_NUM_TRIS * 3] = {
		0,1,2, 2,3,0,
		13,12,14, 14,15,13,
		5,4,7, 7,6,5,
		8,9,11, 11,10,8,
		17,19,18, 18,16,17,
		20,22,23, 23,21,20
	};

	glUseProgram(programID);
	glGenVertexArrays(1, &vaoHandleCube);
	glBindVertexArray(vaoHandleCube);

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(Indices), Indices, GL_STATIC_DRAW);   

	// Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return vaoHandleCube;
}

// Creates a new vertex array object for the floor and loads in data into a vertex attribute buffer
int createFloorVAO(int programID)
{
	// Cube vertex data
	float Vertices[ 4 * VALS_PER_VERT ] = {
		-1.0, -1.0, -1.0,
		1.0, -1.0, -1.0,
		1.0, -1.0, 1.0,
		-1.0, -1.0, 1.0
	};

	// Texture coords for each vertex. 2 per vertex.
	float tex_coord[8] = {
		0.0f, 1.0f,
		1.0f, 1.0f,
		1.0f, 0.0f,
		0.0f, 0.0f,
	};

	// 2 triangles for a square base
	unsigned int Indices[2*3] = {
		0,1,2, 2,3,0
	};

	glUseProgram(programID);
	glGenVertexArrays(1, &vaoHandleFloor);
	glBindVertexArray(vaoHandleFloor);

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(Vertices), Vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Texture attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER, sizeof(tex_coord), tex_coord, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_TEX, GL_FLOAT, GL_FALSE, 0, 0);

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(Indices), Indices, GL_STATIC_DRAW);   

	// Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return vaoHandleFloor;
}

// Creates a new vertex array object for the goal and loads in data into a vertex attribute buffer
int createGoalVAO(int programID)
{
	// Using the sphere class to generate vertices and element indices
	sphere = new Sphere(1.0f, 16, 16);

	glUseProgram(programID);
	glGenVertexArrays(1, &vaoHandleGoal);
	glBindVertexArray(vaoHandleGoal);

	unsigned int buffer[3];
	glGenBuffers(3, buffer);

	// Set vertex position attribute
	glBindBuffer(GL_ARRAY_BUFFER, buffer[0]);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(float) * sphere->vertCount, sphere->vertices, GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, VALS_PER_VERT, GL_FLOAT, GL_FALSE, 0, 0);

	// Normal attributes
	glBindBuffer(GL_ARRAY_BUFFER, buffer[1]);
	glBufferData(GL_ARRAY_BUFFER,
				 sizeof(float) * sphere->normCount, sphere->normals, GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, VALS_PER_NORMAL, GL_FLOAT, GL_FALSE, 0, 0);

	// Set element attributes. Notice the change to using GL_ELEMENT_ARRAY_BUFFER
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, buffer[2]);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER,
				 sizeof(unsigned int) * sphere->indCount, sphere->indices, GL_STATIC_DRAW);

	// Un-bind
	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);
	
	return vaoHandleGoal;
}

// function to load textures
int createTextures()
{
	// A shader program has many texture units,
	// slots in which a texture can be bound, available to
	// it and this function defines which unit we are working with currently
	// We will only use unit 0 here. This is the default.
	glActiveTexture(GL_TEXTURE0);

	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
	glTexParameterf( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );
	
	glGenTextures( NUM_TEXTURES, TexID );
		
	// load an image from file as texture 1
	int x, y, n;
	unsigned char *data1 = stbi_load(
									"crate.jpg", /*char* filepath */
									&x, /*The address to store the width of the image*/
									&y, /*The address to store the height of the image*/
									&n  /*Number of channels in the image*/,
									0   /*Force number of channels if > 0*/
	);

	glBindTexture( GL_TEXTURE_2D, TexID[0] );
	if (n == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data1);
	}
	else {
		fprintf(stderr, "Image pixels are not RGB. Texture may not load correctly.");
	}
	stbi_image_free(data1);
	
	glGenerateMipmap(GL_TEXTURE_2D); 
		
	// load an image from file as texture 2
	unsigned char *data2 = stbi_load(
									"dirt.jpg", /*char* filepath */
									&x, /*The address to store the width of the image*/
									&y, /*The address to store the height of the image*/
									&n  /*Number of channels in the image*/,
									0   /*Force number of channels if > 0*/
	);

	glBindTexture( GL_TEXTURE_2D, TexID[1] );
	if (n == 3) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, x, y, 0, GL_RGB, GL_UNSIGNED_BYTE, data2);
	}
	else {
		fprintf(stderr, "Image pixels are not RGB. Texture may not load correctly.");
	}
	stbi_image_free(data2);
	
	glGenerateMipmap(GL_TEXTURE_2D); 
	
	// create checkerboard image as texture 3
	// We don't really need RGB channels for this image, but use them anyway.
	for(int i=0; i < TEX_SIZE; i++){
		for(int j=0; j < TEX_SIZE; j++)
		{
			GLubyte c;
			c = (((i & 0x8) == 0) ^ ((j & 0x8) == 0)) * 255;
			myimage[i][j][0]  = 255;
			myimage[i][j][1]  = 255;
			myimage[i][j][2]  = 255;
		}
	}

	// load checkerboard image as texture 0
	glBindTexture( GL_TEXTURE_2D, TexID[2] );
	glTexImage2D(GL_TEXTURE_2D, 
				 0,                 // level
				 GL_RGB,            // colour components
				 TEX_SIZE,          // width
				 TEX_SIZE,          // height
				 0,                 // border
				 GL_RGB,            // pixel format
				 GL_UNSIGNED_BYTE,  // pixel type
				 myimage);          // texture data
	
	// generate mipmaps for the texture 
	glGenerateMipmap(GL_TEXTURE_2D); 

	return 0;
}
  
// Constructor - calculates the table's dimensions based on the 1desired height and width
Objects::Objects(int ID)
{
	programID = ID;
	textureEnabled = true;

	vaoHandleCube = createCubeVAO(ID);
	vaoHandleFloor = createFloorVAO(ID);
	vaoHandleGoal = createGoalVAO(ID);
	createTextures();
}

// Draw the table as a set of scaled blocks
void Objects::render()
{
	// glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glUseProgram(programID);

	int modelUniformHandleCube = glGetUniformLocation(programID, "model");
	if (modelUniformHandleCube == -1)
		exit(1);
	int colourLoc = glGetUniformLocation(programID, "colour");
	if (colourLoc == -1)
		exit(1);
	int texHandle = glGetUniformLocation(programID, "texMap");
	if (texHandle == -1)
		exit(1);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER,CurrentTexSample);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER,CurrentTexSample);

	// We are using texture unit 0 (the default)
	glUniform1i(texHandle, 0);

	// glViewport(-size, -size, size, size);
	// glMatrixMode(GL_PROJECTION);
	// glLoadIdentity();
	// change view window dimensions to depend on size to make more intuitive
	glOrtho(0, 2 * size, 0, 2 * size, 0, 2 * size);

	glm::mat4 wall;
	glm::mat4 model;
		
	for (int i = 0; i < size; i++)
	{
		for(int j = 0; j < size; j++)
		{
			if (map[j][i] == '*')
			{	
				// If '*' draw one cube (wall)
				if (textureEnabled)
	    			glBindTexture(GL_TEXTURE_2D, TexID[0]);
	    		else 
	    			glBindTexture(GL_TEXTURE_2D, TexID[2]);
				colour = glm::vec3(1.0f, 1.0f, 1.0f);
				glUniform3fv(colourLoc, 1, glm::value_ptr(colour));
				glBindVertexArray(vaoHandleCube);
				// glbindtexture(texture[i])
				// translate by 2*i because shapes are 2x2x2 in size (-1 to 1)
				model = glm::translate(wall, glm::vec3(-size + 2*(float)i, 0.0f, -size + 2*(float)j));
				glUniformMatrix4fv( modelUniformHandleCube, 1, false, glm::value_ptr(model) );
				glDrawElements(GL_TRIANGLES, CUBE_NUM_TRIS*3, GL_UNSIGNED_INT, 0);
			}
			else 
			{
				// If ' ' draw the floor
				if (textureEnabled)
	    			glBindTexture(GL_TEXTURE_2D, TexID[1]);
	    		else 
	    			glBindTexture(GL_TEXTURE_2D, TexID[2]);
				colour = glm::vec3(0.0f, 1.0f, 0.0f);
				glUniform3fv(colourLoc, 1, glm::value_ptr(colour));
				glBindVertexArray(vaoHandleFloor);
				model = glm::translate(wall, glm::vec3(-size + 2*(float)i, 0.0f, -size + 2*(float)j));
				glUniformMatrix4fv( modelUniformHandleCube, 1, false, glm::value_ptr(model) );
				glDrawElements(GL_TRIANGLES, 2*3, GL_UNSIGNED_INT, 0);
			
				// if 'X' draw the goal
				if (map[j][i] == 'X')
				{

    				glBindTexture( GL_TEXTURE_2D, TexID[2] );
					colour = glm::vec3(1.0f, 0.0f, 0.0f);
					glUniform3fv(colourLoc, 1, glm::value_ptr(colour));
					glBindVertexArray(vaoHandleGoal);
					model = glm::translate(wall, glm::vec3(-size + 2*(float)i, 2.0f, -size + 2*(float)j));
					glUniformMatrix4fv( modelUniformHandleCube, 1, false, glm::value_ptr(model) );
					glDrawElements(GL_TRIANGLES, sphere->indCount, GL_UNSIGNED_INT, 0);
				}
			}
		}
	}	
	
	glBindVertexArray(0);

	glFlush();
}

void Objects::readMap(char *filename)
{
	ifstream file;
	file.open(filename);
	string temp, line;

	// Open the file
	if (file.is_open())
	{
		// Size of map
		getline(file, temp);
		istringstream(temp) >> size;
		map = new char*[size];

		// Read in map 
		for (int i = 0; i < size; i++)
		{
			getline (file, line);
			// Store as stuff
			map[i] = new char[size] ;
			for (int j = 0; j < line.size(); j++)
			{
				map[i][j] = line[j];
			}
		}
		file.close();
	}
}