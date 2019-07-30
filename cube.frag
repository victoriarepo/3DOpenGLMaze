
#version 330

in vec4 pos; // Interpolated vertex position, in object coords
in vec2 st;

uniform sampler2D texMap;

uniform vec3 colour;

// The final colour we will see at this location on screen
out vec4 fragColour;

void main(void)
{
	fragColour = vec4(colour, 1.0f) * texture(texMap, st);
}