#version 330

in vec3 position0;
in vec3 colour0;
in vec3 normal0;

uniform mat4 MVP;
uniform mat4 modelMat;

out vec3 passColour0;
out vec3 passNormal0;
out vec3 passPosition0;

void main()
{
	passColour0 = colour0;
	passNormal0 = (modelMat * vec4(normal0, 0.0)).xyz;
	passPosition0 = (modelMat * vec4(position0, 1.0)).xyz;
	gl_Position = MVP * vec4(position0, 1.0);
}
