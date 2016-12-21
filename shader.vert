// ----------------------------------------------
// Informatique Graphique 3D & Réalité Virtuelle.
// Travaux Pratiques
// Shaders
// Copyright (C) 2015 Tamy Boubekeur
// All rights reserved.
// ----------------------------------------------

varying vec4 P;
varying vec3 N;
varying vec4 C;
varying float shadow;

void main(void) {
    P = gl_Vertex;
    N = gl_Normal;
    C = gl_Color;
	shadow = C[3];
    gl_Position = ftransform ();
}
