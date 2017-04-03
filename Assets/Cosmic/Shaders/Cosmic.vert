#version 330 core

in vec3 position;
in vec3 color;


out vec3 VERTCOLOR;
void main()
{
	VERTCOLOR = color;
	gl_Position = vec4(position, 1);
}