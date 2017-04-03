#version 330

uniform sampler2D texture0;
uniform sampler2D texture1;

uniform float alpha;

in vec2 passTexcoord0;
in vec2 passTexcoord1;

out vec4 fragColor;

void main()
{
	vec4 color = texture(texture0, passTexcoord0) * texture(texture1, passTexcoord1);
	fragColor = vec4(color.rgb, color.a * alpha);
}