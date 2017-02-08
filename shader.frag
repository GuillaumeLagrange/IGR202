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

#define LIGHT_NUMBER 1
#define M_PI 3.14159265359

struct LightSource {
    vec4 pos;
    vec4 color;
    float intensity;
};

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal
varying vec4 C; // fragment-wise normal

uniform vec3 kd;
uniform vec3 matAlbedo;
uniform float alpha;
uniform float f0;
uniform int brdf_mode;
uniform vec3 lightPos;

LightSource lightSources[3];
vec3 diffuse = vec3(C);
vec3 spec = vec3(0.0, 0.0, 0.0);

float fresnel(vec3 wh, vec3 wi);
float dCook(vec3 n, vec3 w);
float gCook(vec3 n, vec3 wh, vec3 wi, vec3 wo);
void cook();
float gGGX(vec3, vec3);
float dGGX(vec3, vec3);
void ggx();

void main (void) {

    lightSources[0].pos = vec4(lightPos, 1.0);
    lightSources[0].color = vec4(1.0, 1.0, 1.0, 1.0);
    lightSources[0].intensity = 3.0;

    lightSources[1].pos = vec4(0.0, 1.5, 0.0, 1.0);
    lightSources[1].color = vec4(0.0, 1.0, 0.0, 1.0);
    lightSources[1].intensity = 3.0;

    lightSources[2].pos = vec4(0.0, 0.0, 1.0, 1.0);
    lightSources[2].color = vec4(0.0, 0.0, 1.0, 1.0);
    lightSources[2].intensity = 3.0;

    if (C.w != 1.0) {
        if (brdf_mode == 0)
            cook();
        else if (brdf_mode == 1)
            ggx();
        else if (brdf_mode == 2){
            diffuse += kd/M_PI;
        vec4 color = vec4((spec + diffuse), 0.0);
        gl_FragColor = color;
        }
    } else {
        gl_FragColor = C.x * vec4(matAlbedo, 1.0) * 0.15;
    }

}

void diffuse()
{
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 wo = normalize (-p);


    /* Model parameters */

    for(int i = 0; i < LIGHT_NUMBER; i++){
        vec3 lightPos = vec3(gl_ModelViewMatrix * lightSources[i].pos);
        vec3 wi = normalize(lightPos - p);
        vec3 wh = normalize(wi + wo);
        vec3 lightColor = vec3(lightSources[i].color);

        /* Attenuation */
        float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));

        /* Diffuse */
        vec3 f_d = kd/M_PI;
        diffuse += attenuation * lightSources[i].intensity *
            lightColor * dot(n, wi) * f_d;
    }
}

float fresnel(vec3 wh, vec3 wi)
{
    return f0 + (1.0-f0)*pow((1.0-max(0.0, dot(wi,wh))),5);
}

float dCook(vec3 n, vec3 w)
{
    float ex = (dot(n, w)*dot(n, w) - 1.0)/(alpha*alpha*dot(n, w)*dot(n, w));
    return (exp(ex))/(M_PI*alpha*alpha*pow(dot(n,w),4));
}

float gCook(vec3 n, vec3 wh, vec3 wi, vec3 wo)
{
    float ombrage = 2.0 * dot(n, wh) * dot(n, wi) / dot(wo, wh);
    float masquage = 2.0 * dot(n, wh) * dot(n, wo) / dot(wo, wh);

    return min(1, min(masquage, ombrage));
}

void cook()
{
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 wo = normalize (-p);


    /* Model parameters */

    for(int i = 0; i < LIGHT_NUMBER; i++){
        vec3 lightPos = vec3(gl_ModelViewMatrix * lightSources[i].pos);
        vec3 wi = normalize(lightPos - p);
        vec3 wh = normalize(wi + wo);
        vec3 lightColor = vec3(lightSources[i].color);

        /* Attenuation */
        float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));

        /* Diffuse */
        vec3 f_d = kd/M_PI;
        diffuse = attenuation * lightSources[i].intensity *
            lightColor * dot(n, wi) * f_d;

        /* Specular */
        float f = fresnel(wh, wi);
        float d = dCook(n, wh);
        float g = gCook(n, wh, wi, wo);
        float f_s = d * f * g / (4.0 * dot(n, wi) * dot(n,wo));
        spec = attenuation * lightSources[i].intensity * dot(n, wi)
            * f_s * lightColor;
    }
}


float gGGX(vec3 n, vec3 w)
{
    float k = alpha * sqrt(2.0 / M_PI);
    float temp = dot(n, w);
    return temp/(temp*(1.0-k)+k);
}

float dGGX(vec3 n, vec3 wh)
{
    float temp = 1.0 + (alpha*alpha - 1.0) * dot(n, wh) * dot(n,wh);
    return alpha*alpha/(M_PI*temp*temp);
}

void ggx()
{
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 wo = normalize (-p);


    /* Model parameters */

    for(int i = 0; i < LIGHT_NUMBER; i++){
        vec3 lightPos = vec3(gl_ModelViewMatrix * lightSources[i].pos);
        vec3 wi = normalize(lightPos - p);
        vec3 wh = normalize(wi + wo);
        vec3 lightColor = vec3(lightSources[i].color);

        /* Attenuation */
        float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));

        /* Diffuse */
        vec3 f_d = kd/M_PI;
        diffuse += attenuation * lightSources[i].intensity *
            lightColor * dot(n, wi) * f_d;

        /* Specular */
        float f = fresnel(wh, wi);
        float d = dGGX(n, wh);
        float g = gGGX(n, wi) * gGGX(n, wo);
        float f_s = d * f * g / (4.0 * dot(n, wi) * dot(n,wo));
        spec += attenuation * lightSources[i].intensity * dot(n, wi)
            * f_s * lightColor;
    }
}

