#version 330

uniform sampler2D texture0;

in vec2 passTexcoord0;
in vec3 passColour0;

out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(texture0, passTexcoord0).rgb * passColour0.rgb, 1);
}
