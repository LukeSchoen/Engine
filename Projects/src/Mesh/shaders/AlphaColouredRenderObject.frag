#version 330

in vec4 passColour0;

out vec4 fragColor;

void main()
{
	fragColor = vec4(passColour0);
}
