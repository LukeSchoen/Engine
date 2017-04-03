#version 330

uniform sampler2D texture0;
uniform sampler2DArray texture1;

uniform float fade;

in vec2 passTexcoord0;
in vec3 passTexcoord1;

out vec4 fragColor;

void main()
{
	vec4 texel = texture(texture0, passTexcoord0);
	vec4 arrayTexel = texture(texture1, passTexcoord1);
	//fragColor = vec4(texel.rgb * arrayTexel.rgb, fade);
	fragColor = vec4(texel.rgb, fade);
}
