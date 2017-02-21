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
#define COOK_MODE 1
#define GGX_MODE 2
#define BLINN_MODE 3

struct LightSource {
    vec4 pos;
    vec4 color;
    float intensity;
};

varying vec4 P; // fragment-wise position
varying vec3 N; // fragment-wise normal
varying vec4 C; // fragment-wise normal

uniform vec3 kd;
uniform vec3 ks;
uniform vec3 matAlbedo;
uniform float alpha;
uniform float f0;
uniform float intensity;
uniform float shininess;
uniform int brdf_mode;
uniform vec3 lightPos;
uniform vec3 lightColor;

LightSource lightSource;
vec3 diffuse = vec3(C);
vec3 spec = vec3(0.0, 0.0, 0.0);

void blinn();
float fresnel(vec3 wh, vec3 wi);
float dCook(vec3 n, vec3 w);
float gCook(vec3 n, vec3 wh, vec3 wi, vec3 wo);
void cook();
float gGGX(vec3, vec3);
float dGGX(vec3, vec3);
void ggx();

void main (void) {
    gl_FragColor = vec4 (0.0, 0.0, 0.0, 1.0);

    lightSource.pos = vec4(lightPos, 1.0);
    lightSource.color = vec4(lightColor, 1.0);
    lightSource.intensity = intensity;

    if (brdf_mode == COOK_MODE)
        cook();
    if (brdf_mode == GGX_MODE)
        ggx();
    if (brdf_mode == BLINN_MODE)
        blinn();

    vec4 color = vec4((spec + diffuse), 1.0);

    if (C.w > 0.0)
        color *= 15.0 * C.w;
    else
        color *= -5.0 * C.w;

    gl_FragColor += color;
}

void blinn()
{
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 wo = normalize (-p);

    vec3 lightPos = vec3(gl_ModelViewMatrix * lightSource.pos);
    vec3 wi = normalize(lightPos - p);
    vec3 wh = normalize(wi + wo);
    vec3 lightColor = vec3(lightSource.color);

    /* Attenuation */
    float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));

    /* Diffuse */
    vec3 f_d = kd/M_PI;
    diffuse += attenuation * lightSource.intensity *
        lightColor * dot(n, wi)* matAlbedo * f_d;

    /* Specular */
    vec3 r = 2.0*dot(wi, n)*n - wi;
    vec3 f_s = ks * pow(dot(r, wo), shininess);
    spec += attenuation * lightSource.intensity * dot(n, wi)
        * f_s * lightColor;
}

float fresnel(vec3 wh, vec3 wi)
{
    return f0 + (1.0-f0)*pow((1.0-max(0.0, dot(wi,wh))),5);
}

float dCook(vec3 n, vec3 wh)
{
    return max(0.0, exp((pow(dot(n, wh), 2) - 1.0)/pow(alpha*dot(n, wh), 2))/
        (M_PI * pow(alpha, 2) * pow(dot(n, wh), 2)));
}

float gCook(vec3 n, vec3 wh, vec3 wi, vec3 wo)
{
    float nDotOmega0 = max(0.0, dot(n, wo));
    float nDotOmegaI = max(0.0, dot(n, wi));
    float nDotOmegaH = max(0.0, dot(n, wh));
    float omega0DotOmegaH = max(0.0, dot(wo, wh));
    float omegaIDotOmegaH = max(0.0, dot(wh, wi));

    return min(1.0,min(2.0*nDotOmegaH*nDotOmegaI/omega0DotOmegaH,2.0*nDotOmega0*nDotOmegaH/omega0DotOmegaH));
}

void cook()
{
    vec3 p = vec3 (gl_ModelViewMatrix * P);
    vec3 n = normalize (gl_NormalMatrix * N);
    vec3 wo = normalize (-p);

    vec3 lightPos = vec3(gl_ModelViewMatrix * lightSource.pos);
    vec3 wi = normalize(lightPos - p);
    vec3 wh = normalize(wi + wo);
    vec3 lightColor = vec3(lightSource.color);

    /* Attenuation */
    float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));

    /* Diffuse */
    vec3 f_d = kd/M_PI;
    diffuse += attenuation * lightSource.intensity *
        lightColor * dot(n, wi) * matAlbedo * f_d;

    /* Specular */
    //float f = 1.0;
    //float d = 1.0;
    //float g = 1.0;
    float f = fresnel(wh, wi);
    float d = dCook(n, wh);
    float g = gCook(n, wh, wi, wo);
    float f_s = d * f * g / (4.0 * dot(n, wi) * dot(n,wo));
    spec += attenuation * lightSource.intensity * dot(n, wi)
        * f_s * lightColor;
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

    vec3 lightPos = vec3(gl_ModelViewMatrix * lightSource.pos);
    vec3 wi = normalize(lightPos - p);
    vec3 wh = normalize(wi + wo);
    vec3 lightColor = vec3(lightSource.color);

    /* Attenuation */
    float attenuation = 1.0/(length(p - lightPos) * length(p - lightPos));

    /* Diffuse */
    vec3 f_d = kd/M_PI;
    diffuse += attenuation * lightSource.intensity *
        lightColor * dot(n, wi) * matAlbedo * f_d;

    /* Specular */
    float f = fresnel(wh, wi);
    float d = dGGX(n, wh);
    float g = gGGX(n, wi) * gGGX(n, wo);
    float f_s = d * f * g / (4.0 * dot(n, wi) * dot(n,wo));
    spec += attenuation * lightSource.intensity * dot(n, wi)
        * f_s * lightColor;
}
