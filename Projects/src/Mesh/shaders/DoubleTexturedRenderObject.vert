#version 330

in vec3 position0;
in vec2 texcoord0;
in vec2 texcoord1;

uniform mat4 MVP;

out vec2 passTexcoord0;
out vec2 passTexcoord1;

void main()
{
	passTexcoord0 = texcoord0;
	passTexcoord1 = texcoord1;
	gl_Position = MVP * vec4(position0, 1.0);
}
