#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;

in vec2 passTexcoord0;
in vec2 passTexcoord1;

out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(texture0, passTexcoord0).rgb * texture(texture1, passTexcoord1).rgb, 1.0);
}
