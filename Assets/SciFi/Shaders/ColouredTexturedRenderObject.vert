#version 330

in vec3 position0;
in vec2 texcoord0;
in vec3 colour0;

uniform mat4 MVP;

out vec2 passTexcoord0;
out vec3 passColour0;

void main()
{
	passTexcoord0 = texcoord0;
	passColour0 = colour0;
	gl_Position = MVP * vec4(position0, 1.0);
}
