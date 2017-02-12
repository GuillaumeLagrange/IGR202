// --------------------------------------------------------------------------
// Copyright(C) 2009-2016
// Tamy Boubekeur
//
// Permission granted to use this code only for teaching projects and
// private practice.
//
// Do not distribute this code outside the teaching assignements.
// All rights reserved.
// --------------------------------------------------------------------------

#include <GL/glew.h>
#include <GL/glut.h>
#include <iostream>
#include <vector>
#include <string>
#include <cstdio>
#include <cstdlib>
#include <algorithm>
#include <cmath>
#include <random>

#include "Vec3.h"
#include "Camera.h"
#include "Mesh.h"
#include "GLProgram.h"
#include "Exception.h"
#include "LightSource.h"
#include "Ray.h"
#include "BoundingBox.h"
#include "BVH.h"

using namespace std;

#define ALPHA 0.8
#define FZERO 0.02
#define KD 1.0,1.0,1.0
#define KS 1.0,1.0,1.0
#define ALBEDO 0.15,0.15,0.15
#define SHININESS 1.0
#define COOK_MODE 1
#define GGX_MODE 2
#define BLINN_MODE 3
#define LIGHT_POS 1.0,0.0,0.0
#define LIGHT_COL 1.0,0.0,0.0
#define LIGHT_INT 1.0
#define EPSILON 0.0001f

static const unsigned int DEFAULT_SCREENWIDTH = 1024;
static const unsigned int DEFAULT_SCREENHEIGHT = 768;
static const string DEFAULT_MESH_FILE ("models/monkey.off");

static const string appTitle ("Informatique Graphique & Realite Virtuelle - Travaux Pratiques - Algorithmes de Rendu");
static const string myName ("Guillaume Lagrange");
static GLint window;
static unsigned int FPS = 0;
static bool fullScreen = false;

static Camera camera;
static Mesh mesh;
GLProgram * glProgram;

int brdf_mode;
float shininess;
float alpha;
float f0;
Vec3f kd;
Vec3f ks;
Vec3f matAlbedo;
GLuint vertexVBO;
GLuint indexVBO;
GLuint normalVBO;
GLuint colorVBO;
static BVH * bvh;
static LightSource lightSource;
static std::vector<float> colorResponses; // Cached per-vertex color response, updated at each frame

void printUsage () {
    std::cerr << std::endl
        << appTitle << std::endl
        << "Author: " << myName << std::endl << std::endl
        << "Usage: ./main [<file.off>]" << std::endl
        << "Commands:" << std::endl
        << "------------------" << std::endl
        << " ?: Print help" << std::endl
        << " w: Toggle wireframe mode" << std::endl
        << " <drag>+<left button>: rotate model" << std::endl
        << " <drag>+<right button>: move model" << std::endl
        << " <drag>+<middle button>: zoom" << std::endl
        << " <esc>: Quit" << std::endl
        << " z, q, s, d : Move the light source position" << std::endl
        << " 1, 2, 3 : GGX, Cook, Blinn mode" << std::endl
        << " r : Red light source" << std::endl
        << " g : Green light source" << std::endl
        << " b : Blue light source" << std::endl
        << " v : White light source" << std::endl
        << " i, I : Control the intensity of the light source" << std::endl
        << " t : Compute per vertex shadow" << std::endl
        << " o : Compute per vertex AO" << std::endl << std::endl;
}

/* This function updates the shadow value in colorResponses by ray tracing */
void computePerVertexShadow()
{
    Vec3f lightPos = lightSource.getPosition();
    std::vector<Vec3f> positions = mesh.positions();
    std::vector<Triangle> triangles = mesh.triangles();

    for (unsigned int i = 0; i < positions.size(); i++) {
        Ray ray = Ray(positions[i], lightPos - positions[i]);
        colorResponses[4*i+3] = 1.0;

        for (unsigned int j = 0; j<triangles.size(); j++) {
            if (!triangles[j].contains(i)) {
                int i0 = triangles[j][0];
                int i1 = triangles[j][1];
                int i2 = triangles[j][2];
                if (ray.rayTriangleInter(positions[i0], positions[i1],
                            positions[i2])) {
                    colorResponses[4*i+3] = -1.0;
                }
            }
        }
    }

    /* Updating the VBO, sending values to GPU */
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colorResponses.size() * sizeof(float),
            &(colorResponses[0]), GL_DYNAMIC_DRAW);
}

void computePerVertexAO(int numOfSamples, float radius)
{
    std::random_device rd;
    std::default_random_engine generator(rd());
    std::uniform_real_distribution<float> range(-1.f,1.f);

    std::vector<Vec3f> positions = mesh.positions();
    std::vector<Vec3f> normals = mesh.normals();
    std::vector<Triangle> triangles = mesh.triangles();

    for (unsigned int i = 0; i < positions.size(); i++) {
        Vec3f x,y;
        Vec3f position = positions[i];
        Vec3f normal = normalize(normals[i]);
        normal.getTwoOrthogonals(x, y);

        float ao = 0;
        for (int j = 0; j < numOfSamples; j++) {
            /* Generation of a random vector */
            float u = range(generator);
            float v = range(generator);
            Vec3f w = normalize(u*x + v*y + normal);

            Ray ray = Ray(position, w);
            bool inter = false;

            for(unsigned int k = 0; k < triangles.size(); k++) {
                if (!triangles[k].contains(i)) {
                    int i0 = triangles[k][0];
                    int i1 = triangles[k][1];
                    int i2 = triangles[k][2];
                    Vec3f t = (positions[i0]+positions[i1]+positions[i2])/3.f;
                    float dist = length(position - t);
                    inter |= ray.rayTriangleInter(positions[i0],
                            positions[i1], positions[i2]) && (dist < radius);
                }
            }

            if(!inter)
                ao += dot(normal, w);
        }

        ao *= 1.f/(float) numOfSamples;

        /* Multiplication of albedo by AO factor */
        colorResponses[4*i + 3] *= ao;
    }

    /* Updating the VBO, sending values to GPU */
    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colorResponses.size() * sizeof(float),
            &(colorResponses[0]), GL_DYNAMIC_DRAW);
}

void init (const char * modelFilename) {
    glewExperimental = GL_TRUE;
    glewInit (); // init glew, which takes in charges the modern OpenGL calls (v>1.2, shaders, etc)
    glCullFace (GL_BACK);     // Specifies the faces to cull (here the ones pointing away from the camera)
    glEnable (GL_CULL_FACE); // Enables face culling (based on the orientation defined by the CW/CCW enumeration).
    glDepthFunc (GL_LESS); // Specify the depth test for the z-buffer
    glEnable (GL_DEPTH_TEST); // Enable the z-buffer in the rasterization
    glEnableClientState (GL_VERTEX_ARRAY);
    glEnableClientState (GL_NORMAL_ARRAY);
    glEnableClientState (GL_COLOR_ARRAY);
    glEnable (GL_NORMALIZE);
    glLineWidth (2.0); // Set the width of edges in GL_LINE polygon mode
    glClearColor (0.0f, 0.0f, 0.0f, 1.0f); // Background color
    mesh.loadOFF (modelFilename);
    colorResponses.resize (4 * mesh.positions().size(), 0.0f);
    camera.resize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
    try {
        glProgram = GLProgram::genVFProgram ("Simple GL Program", "shader.vert", "shader.frag"); // Load and compile pair of shaders
        glProgram->use (); // Activate the shader program

    } catch (Exception & e) {
        cerr << e.msg () << endl;
    }

    /* Constant initialization */
    brdf_mode = COOK_MODE;
    f0 = FZERO;
    alpha = alpha;
    shininess = SHININESS;
    kd = Vec3f(KD);
    ks = Vec3f(KS);
    matAlbedo = Vec3f(ALBEDO);
    lightSource = LightSource(Vec3f(LIGHT_POS), Vec3f(LIGHT_COL), LIGHT_INT);
    Vec3f lightPos = lightSource.getPosition();
    Vec3f lightColor = lightSource.getColor();
    float intensity = lightSource.getIntensity();

    /* Uniform initialization */
    glProgram->setUniform3f("kd", kd[0], kd[1], kd[2]);
    glProgram->setUniform3f("ks", ks[0], ks[1], ks[2]);
    glProgram->setUniform3f("matAlbedo", matAlbedo[0], matAlbedo[1],
            matAlbedo[2]);
    glProgram->setUniform3f("lightPos", lightPos[0], lightPos[1], lightPos[2]);
    glProgram->setUniform3f("lightColor", lightColor[0], lightColor[1],
            lightColor[2]);

    glProgram->setUniform1f("alpha", alpha);
    glProgram->setUniform1f("f0", f0);
    glProgram->setUniform1f("intensity", intensity);
    glProgram->setUniform1f("shininess", shininess);

    glProgram->setUniform1i("brdf_mode", brdf_mode);

    /* Settting 4th compenent of colors as 1 */
    for (unsigned int i = 0; i < mesh.positions().size(); i++) {
        colorResponses[4*i + 3] = 1.f;
    }

    /* VBO setup */
    glGenBuffers(1, &vertexVBO);
    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.positions().size() * sizeof(Vec3f),
            &(mesh.positions()[0]), GL_STATIC_DRAW);

    glGenBuffers(1, &indexVBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, mesh.triangles().size() * sizeof(Triangle),
            &(mesh.triangles()[0]), GL_STATIC_DRAW);

    glGenBuffers(1, &normalVBO);
    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glBufferData(GL_ARRAY_BUFFER, mesh.normals().size() * sizeof(Vec3f),
            &(mesh.normals()[0]), GL_STATIC_DRAW);

    glGenBuffers(1, &colorVBO);
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glBufferData(GL_ARRAY_BUFFER, colorResponses.size() * sizeof(float),
            &(colorResponses[0]), GL_DYNAMIC_DRAW);
}

void renderScene () {
    glBindBuffer(GL_ARRAY_BUFFER, colorVBO);
    glColorPointer(4, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, vertexVBO);
    glVertexPointer(3, GL_FLOAT, 0, 0);

    glBindBuffer(GL_ARRAY_BUFFER, normalVBO);
    glNormalPointer(GL_FLOAT, 0, 0);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, indexVBO);
    glDrawElements(GL_TRIANGLES, 3*mesh.triangles().size(), GL_UNSIGNED_INT, 0);
}

void reshape(int w, int h) {
    camera.resize (w, h);
}

void display () {
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    camera.apply ();
    renderScene ();
    glFlush ();
    glutSwapBuffers ();
}

void key (unsigned char keyPressed, int x, int y) {
    switch (keyPressed) {
    case 'f':
        if (fullScreen) {
            glutReshapeWindow (camera.getScreenWidth (), camera.getScreenHeight ());
            fullScreen = false;
        } else {
            glutFullScreen ();
            fullScreen = true;
        }
        break;
    case 'z': {
        lightSource.addTheta(0.1);
        Vec3f lightPos = lightSource.getPosition();
        glProgram->setUniform3f("lightPos",
            lightPos[0], lightPos[1], lightPos[2]);
        break;
        }
    case 's': {
        lightSource.addTheta(-0.1);
        Vec3f lightPos = lightSource.getPosition();
        glProgram->setUniform3f("lightPos",
            lightPos[0], lightPos[1], lightPos[2]);
        break;
        }
    case 'q': {
        lightSource.addPhi(0.1);
        Vec3f lightPos = lightSource.getPosition();
        glProgram->setUniform3f("lightPos",
            lightPos[0], lightPos[1], lightPos[2]);
        break;
        }
    case 'd': {
        lightSource.addPhi(-0.1);
        Vec3f lightPos = lightSource.getPosition();
        glProgram->setUniform3f("lightPos",
            lightPos[0], lightPos[1], lightPos[2]);
        break;
        }
    case 'r': {
        lightSource.setColor(Vec3f(1.0,0.0,0.0));
        Vec3f lightColor = lightSource.getColor();
        glProgram->setUniform3f("lightColor", lightColor[0], lightColor[1],
                lightColor[2]);
        break;
        }
    case 'g': {
        lightSource.setColor(Vec3f(0.0,1.0,0.0));
        Vec3f lightColor = lightSource.getColor();
        glProgram->setUniform3f("lightColor", lightColor[0], lightColor[1],
                lightColor[2]);
        break;
        }
    case 'b': {
        lightSource.setColor(Vec3f(0.0,0.0,1.0));
        Vec3f lightColor = lightSource.getColor();
        glProgram->setUniform3f("lightColor", lightColor[0], lightColor[1],
                lightColor[2]);
        break;
        }
    case 'v': {
        lightSource.setColor(Vec3f(0.5,0.5,0.5));
        Vec3f lightColor = lightSource.getColor();
        glProgram->setUniform3f("lightColor", lightColor[0], lightColor[1],
                lightColor[2]);
        break;
        }
    case 'i': {
        lightSource.addIntensity(-0.05);
        float intensity = lightSource.getIntensity();
        glProgram->setUniform1f("intensity", intensity);
        break;
        }
    case 'I': {
        lightSource.addIntensity(0.05);
        float intensity = lightSource.getIntensity();
        glProgram->setUniform1f("intensity", intensity);
        break;
        }
    case '1': {
        brdf_mode = GGX_MODE;
        glProgram->setUniform1i("brdf_mode", brdf_mode);
        break;
        }
    case '2': {
        brdf_mode = COOK_MODE;
        glProgram->setUniform1i("brdf_mode", brdf_mode);
        break;
        }
    case '3': {
        brdf_mode = BLINN_MODE;
        glProgram->setUniform1i("brdf_mode", brdf_mode);
        break;
        }
    case 27:
        exit (0);
        break;
    case 'w':
        GLint mode[2];
        glGetIntegerv (GL_POLYGON_MODE, mode);
        glPolygonMode (GL_FRONT_AND_BACK, mode[1] ==  GL_FILL ? GL_LINE : GL_FILL);
        break;
        break;
    case 't' :
        computePerVertexShadow();
        break;
    case 'o' :
        computePerVertexAO(100, 1.0);
        break;
    case 'n' :
        bvh->draw(colorResponses);
        break;
    case 'm' :
        bvh = new BVH(mesh);
        break;
    default:
        printUsage ();
        break;
    }
}

void mouse (int button, int state, int x, int y) {
    camera.handleMouseClickEvent (button, state, x, y);
}

void motion (int x, int y) {
    camera.handleMouseMoveEvent (x, y);
}

void idle () {
    static float lastTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    static unsigned int counter = 0;
    counter++;
    float currentTime = glutGet ((GLenum)GLUT_ELAPSED_TIME);
    if (currentTime - lastTime >= 1000.0f) {
        FPS = counter;
        counter = 0;
        static char winTitle [128];
        unsigned int numOfTriangles = mesh.triangles ().size ();
        sprintf (winTitle, "Number Of Triangles: %d - FPS: %d", numOfTriangles, FPS);
        string title = appTitle + " - By " + myName  + " - " + winTitle;
        glutSetWindowTitle (title.c_str ());
        lastTime = currentTime;
    }
    glutPostRedisplay ();
}

int main (int argc, char ** argv) {
    if (argc > 2) {
        printUsage ();
        exit (1);
    }
    glutInit (&argc, argv);
    glutInitDisplayMode (GLUT_RGBA | GLUT_DEPTH | GLUT_DOUBLE);
    glutInitWindowSize (DEFAULT_SCREENWIDTH, DEFAULT_SCREENHEIGHT);
    window = glutCreateWindow (appTitle.c_str ());
    init (argc == 2 ? argv[1] : DEFAULT_MESH_FILE.c_str ());
    glutIdleFunc (idle);
    glutReshapeFunc (reshape);
    glutDisplayFunc (display);
    glutKeyboardFunc (key);
    glutMotionFunc (motion);
    glutMouseFunc (mouse);
    printUsage ();
    glutMainLoop ();
    return 0;
}
