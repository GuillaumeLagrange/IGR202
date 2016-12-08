// ----------------------------------------------
// Informatique Graphique 3D & Réalité Virtuelle.
// Travaux Pratiques
// Shaders
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

// Add here all the value you need to describe the light or the material.
// At first used const values.
// Then, use uniform variables and set them from the CPU program.

#define LIGHT_NUMBER 3
#define ALPHA_VALUE 0.5
#define F0_VALUE 0.2
#define M_PI 3.14159265359

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal
varying vec4 C; // fragment-wise normal

struct LightSource {
	vec4 pos;
	vec4 color;
	float intensity;
};

float fresnel(vec3 wh, vec3 wi);
float dCook(vec3 n, vec3 w);
float gCook(vec3 n, vec3 wh, vec3 wi, vec3 wo);


void main (void) {
    gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0);

	LightSource lightSources[LIGHT_NUMBER];

	lightSources[0].pos = vec4(1.0, 0.0, 0.0, 1.0);
	lightSources[0].color = vec4(1.0, 0.0, 0.0, 1.0);
	lightSources[0].intensity = 2.0;

	lightSources[1].pos = vec4(0.0, 1.0, 0.0, 1.0);
	lightSources[1].color = vec4(0.0, 1.0, 0.0, 1.0);
	lightSources[1].intensity = 2.0;

	lightSources[2].pos = vec4(0.0, 0.0, 1.0, 1.0);
	lightSources[2].color = vec4(0.0, 0.0, 1.0, 1.0);
	lightSources[2].intensity = 2.0;

    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 v = normalize (-p);

    vec4 color = C;

	vec3 camPos = vec3(gl_ModelViewMatrix *  gl_ModelViewMatrixInverse[3]);
	vec3 wo = normalize(camPos - p);

	float alpha = ALPHA_VALUE;
	float f0 = F0_VALUE;

	for(int i = 0; i < LIGHT_NUMBER; i++){
		vec3 lightPos = vec3(gl_ModelViewMatrix * lightSources[i].pos);
		vec3 wi = normalize(p - lightPos);
		vec3 wh = normalize(wi + wo);

		float f = fresnel(wh, wi);
		float d = dCook(n, wh);
		float g = gCook(n, wh, wi, wo);
		float f_s = d * f * g / (4.0 * dot(n, wi) * dot(n,wo));
		float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));
		color += attenuation * lightSources[i].intensity * dot(n, wi) * f_s
			* lightSources[i].color;
	}

    gl_FragColor += color;
}

float fresnel(vec3 wh, vec3 wi)
{
	float f0 = F0_VALUE;
	return f0 + (1.0-f0)*pow((1.0-max(0.0, dot(wi,wh))),5);
}

float dCook(vec3 n, vec3 w)
{
	float alpha = ALPHA_VALUE;
	float ex = (dot(n, w)*dot(n, w) - 1.0)/(alpha*alpha*dot(n, w)*dot(n, w));
	return (exp(ex))/(M_PI*alpha*alpha*pow(dot(n,w),4));
}

float gCook(vec3 n, vec3 wh, vec3 wi, vec3 wo)
{
	float ombrage = 2.0*dot(n, wh)*dot(n, wi)/dot(wo, wh);
	float masquage = 2.0 * dot(n, wh) * dot(n, wo) / dot(wo, wh);

	return min(1, min(masquage, ombrage));
}
