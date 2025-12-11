#version 330 core
#define MAX_LIGHTS 16

in vec3 vPosition;
in vec3 vNormal;
in vec2 vTexCoord0;
in vec2 vTexCoord1;
out vec4 fragColor;

struct Light
{
	vec3 position;
	vec3 color;
	float intensity;
	float range;
};

uniform Light uLights[MAX_LIGHTS];
uniform int uLightCount;

uniform vec3 uDiffuseColor;
uniform bool uUseLightmap;
uniform sampler2D uDiffuseTexture;
uniform sampler2D uLightmapTexture;


float dither4x4[16] = float[16](
    0.0,  8.0,  2.0, 10.0,
   12.0,  4.0, 14.0,  6.0,
    3.0, 11.0,  1.0,  9.0,
   15.0,  7.0, 13.0,  5.0
);

float bayerThreshold(vec2 fragCoord) {
    int x = int(mod(fragCoord.x, 4.0));
    int y = int(mod(fragCoord.y, 4.0));
    return dither4x4[y * 4 + x] / 16.0;
}

void main()
{
	float ambient = 0.5;
	vec4 baseColor = texture(uDiffuseTexture, vTexCoord0);
	vec3 color = baseColor.rgb * uDiffuseColor;

	if (uUseLightmap)
	{
		vec3 lightmap = texture(uLightmapTexture, vTexCoord1).rgb;
		color *= lightmap;
	}
	else
	{
		vec3 lightColor = vec3(ambient);
		for (int i = 0; i < uLightCount; ++i)
		{
			vec3 lightLine = uLights[i].position - vPosition;
			vec3 lightDirection = normalize(lightLine);
			float lightDistance = length(lightLine);
			float lightFalloff = clamp(1.0 - (lightDistance / uLights[i].range), 0.0, 1.0);

			float ndotL = max(dot(vNormal, lightDirection), 0.0);
			lightColor += uLights[i].color * uLights[i].intensity * lightFalloff * ndotL;
		}
		color *= lightColor;
	}

	float threshold = bayerThreshold(gl_FragCoord.xy);

	color = floor(color * 31.0 + threshold) / 31.0; // 5-bit quantization
	fragColor = vec4(color, baseColor.a);
}