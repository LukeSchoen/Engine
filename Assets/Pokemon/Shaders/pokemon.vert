#version 330 core

in vec3 position;
in vec2 uvs;

out vec2 passTexcoord0;
void main()
{
	passTexcoord0 = uvs;
	gl_Position = vec4(position, 1);
}