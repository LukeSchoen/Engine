#version 330

uniform sampler2D texture0;

in vec2 passTexcoord0;
in vec3 passNormal0;
in vec3 passPosition0;

out vec4 fragColor;

uniform vec3 camPos;
uniform vec3 camDir;

void main()
{
  vec3 normal = normalize(passNormal0);
  
  float specularDistance = 0.1;
  vec3 lightDirection = normalize(vec3(0.5, -1, 0));
  
  // Ambient
  float ambientIntensity = 0.5;
  
  // Diffuse
  float diffuseIntensity = max(dot(normal, -camDir), dot(normal, camDir));
  vec3 diffuse = vec3(texture(texture0, passTexcoord0));
 
  // Specular
  vec3 fragmentToCameraDirection = normalize(-camPos - passPosition0);
  vec3 lightReflection = reflect(lightDirection, normal);
  float specularIntensity = smoothstep(1.0 - specularDistance, 1.0, min(abs(dot(fragmentToCameraDirection, reflect(lightDirection, normal))), 1.0f	));
  vec3 specular = vec3(1.0, 0.75, 0.675);
  
  //fragColor = vec4((ambientIntensity + diffuseIntensity) * diffuse + specularIntensity * specular * 1.2, 1.0);
  fragColor = vec4((ambientIntensity + diffuseIntensity * 0.65) * diffuse + specularIntensity * specular * 1.2, 1.0);
}
