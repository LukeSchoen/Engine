#version 330 core

uniform mat4 MVP;
uniform float LAYER;
uniform vec3 regionPos;
in vec3 position0;
in vec3 texcoord0;
in vec3 colour0;
out vec3 passTexcoord0;
out vec3 passColour0;

void main()
{
	passTexcoord0 = texcoord0;
	passColour0 = colour0;
	gl_Position = MVP * vec4((position0 + regionPos) * LAYER, 1);
}