#ifndef OBJECTS_H
#define OBJECTS_H
/**
 This class draws a table by scaling and moving blocks.
 The table top is the plane y = 0
*/

class Objects
{
private:
	int programID;
	bool textureEnabled;
	
public:
	char **map;
	int size;
	Objects(int ID);
	void render();
	void readMap(char *filename);
	int getSize();
	void ToggleTexture();
};

#endif