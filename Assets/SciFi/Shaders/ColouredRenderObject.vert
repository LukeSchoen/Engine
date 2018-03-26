#version 330

in vec3 position0;
in vec3 colour0;

uniform mat4 MVP;

out vec3 passColour0;

void main()
{
	passColour0 = colour0;
	gl_Position = MVP * vec4(position0, 1.0);
}
