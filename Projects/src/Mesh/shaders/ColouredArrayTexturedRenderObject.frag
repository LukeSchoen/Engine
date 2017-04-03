#version 330

uniform sampler2DArray texture0;

uniform float fade;

in vec3 passTexcoord0;
in vec3 passColour0;

out vec4 fragColor;

void main()
{
	fragColor = vec4(texture(texture0, passTexcoord0).rgb * passColour0.rgb, fade);
}
