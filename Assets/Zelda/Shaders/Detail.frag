#version 330

uniform float useDetail;

uniform sampler2D texture0;
in vec2 passTexcoord0;

out vec4 fragColor;

void main()
{
	vec4 tex0 = vec4(texture(texture0, passTexcoord0).rgb, 1);
	vec4 tex1 = vec4(texture(texture0, passTexcoord0 * 2).rgb, 1);
	vec4 tex2 = vec4(texture(texture0, passTexcoord0 * 4).rgb, 1);
	vec4 tex3 = vec4(texture(texture0, passTexcoord0 * 8).rgb, 1);
	vec4 tex4 = vec4(texture(texture0, passTexcoord0 * 16).rgb, 1);
	vec4 tex5 = vec4(texture(texture0, passTexcoord0 * 32).rgb, 1);
	vec4 tex6 = vec4(texture(texture0, passTexcoord0 * 64).rgb, 1);
	
	float mult = 8;
	
	float depth0 = 1.0 - pow(gl_FragCoord.z, 60 * mult);
	float depth1 = 1.0 - pow(gl_FragCoord.z, 50 * mult);
	float depth2 = 1.0 - pow(gl_FragCoord.z, 40 * mult);
	float depth3 = 1.0 - pow(gl_FragCoord.z, 30 * mult);
	float depth4 = 1.0 - pow(gl_FragCoord.z, 20 * mult);
	float depth5 = 1.0 - pow(gl_FragCoord.z, 10 * mult);
	
	
	fragColor = tex0 * (1-depth0) + tex1 * depth0;
	fragColor = fragColor * (1-depth1) + tex2 * depth1;
	fragColor = fragColor * (1-depth2) + tex3 * depth2;
	fragColor = fragColor * (1-depth3) + tex4 * depth3;
	fragColor = fragColor * (1-depth4) + tex5 * depth4;
	fragColor = fragColor * (1-depth5) + tex6 * depth5;
	
	fragColor = tex0;
}
