#version 330

uniform sampler2D texture0;
in vec2 passTexcoord0;
out vec4 fragColor;


float brightness = 1.0;


void main()
{
	fragColor = vec4(texture(texture0, passTexcoord0).rgb * brightness, 1);
}
