#version 330

in vec3 position0;
in vec2 texcoord0;

uniform mat4 MVP;

out vec2 passTexcoord0;

out float d;

void main()
{
	passTexcoord0 = texcoord0;
	gl_Position = MVP * vec4(position0, 1.0);
	d = gl_Position.z;
}
