#version 330

in vec3 passColour0;

out vec4 fragColor;

void main()
{
	fragColor = vec4(passColour0.rgb, 1.0);
}
