#version 330

uniform sampler2D texture0;

in vec2 passTexcoord0;

out vec4 fragColor;

void main()
{
	vec4 color = texture(texture0, passTexcoord0);
	fragColor = vec4(color.rgb, color.a);
}