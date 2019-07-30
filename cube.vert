
#version 330

// Position and colour variables. 1 per vertex.
/*layout (location = 0) */in vec3 a_vertex; 
/*layout (location = 1) */in vec2 a_tex_coord;
//layout (location = 1) in vec4 a_colour;	

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

// This colour attribute will be sent out per-vertex and automatically
// interpolated between vertices connected to the same triangle
out vec4 pos;
out vec2 st;

void main(void)
{
	// if (colour == 1)
	// 	pos = vec4(1.0f, 0.0f, 0.0f, 1.0f);	// We simply pass the colour along to the next stage
	// else 
	// 	pos = vec4(0.0f, 0.0f, 1.0f, 1.0f);
	pos = vec4(a_vertex, 1.0f);
	st = a_tex_coord;

    // a_vertex is vertex position in object coordinates
    // gl_position is vertex position in camera coordinates
	gl_Position = projection * view * model * vec4(a_vertex, 1.0f);
}
