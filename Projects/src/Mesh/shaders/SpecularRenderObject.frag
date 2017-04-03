#version 330

in vec3 passColour0;
in vec3 passNormal0;
in vec3 passPosition0;

out vec4 fragColor;

uniform vec3 camPos;
uniform vec3 camDir;

void main()
{
  vec3 normal = normalize(passNormal0);
  
	float specularDistance = 0.3;
	vec3 lightDirection = normalize(vec3(0.5, -1, 0));
	
	// Diffuse
	float diffuseIntensity = max(dot(normal, -lightDirection), dot(normal, lightDirection));
	vec3 diffuse = passColour0;
	
	// Specular
	vec3 fragmentToCameraDirection = normalize(-camPos - passPosition0);
	vec3 lightReflection = reflect(lightDirection, normal);
	float specularIntensity = smoothstep(1.0 - specularDistance, 1.0, clamp(dot(fragmentToCameraDirection, reflect(lightDirection, normal)), 0.0, 1.0));
	vec3 specular = vec3(1.0, 0.75, 0.675);
	
	fragColor = vec4(diffuseIntensity * diffuse + specularIntensity * specular * 1.2, 1.0);
}
