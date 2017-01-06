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

#include "Mesh.h"
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <string>
#define PLANE true
#define HEIGHT 10.0
#define WIDTH 5.0
#define HEIGHT_RES 100
#define WIDTH_RES 50
#define YVALUE -3.0

using namespace std;

void Mesh::clear () {
    m_positions.clear ();
    m_normals.clear ();
    m_triangles.clear ();
}

void Mesh::loadOFF (const std::string & filename) {
    clear ();
	ifstream in (filename.c_str ());
    if (!in)
        exit (1);
	string offString;
    unsigned int sizeV, sizeT, tmp;
    in >> offString >> sizeV >> sizeT >> tmp;
    m_positions.resize (sizeV);
    m_triangles.resize (sizeT);
    for (unsigned int i = 0; i < sizeV; i++)
        in >> m_positions[i];
    int s;
    for (unsigned int i = 0; i < sizeT; i++) {
        in >> s;
        for (unsigned int j = 0; j < 3; j++)
            in >> m_triangles[i][j];
    }
    in.close ();

    if (PLANE) {
        for(int i = 0; i < HEIGHT_RES; i++) {
            for(int j = 0; j < WIDTH_RES; j++) {
                float x = (float) i * HEIGHT / (float) HEIGHT_RES - HEIGHT / 2;
                float z = (float) j * WIDTH / (float) WIDTH_RES - WIDTH / 2;
                m_positions.push_back(Vec3f(x, YVALUE, z));

                if ((i != HEIGHT_RES -1) && (j != WIDTH_RES - 1)) {
                    unsigned int i0 = m_positions.size() - 1;
                    unsigned int i1 = i0 + 1;
                    unsigned int i2 = i0 + WIDTH_RES ;
                    unsigned int i3 = i0 + WIDTH_RES + 1;
                    m_triangles.push_back(Triangle(i0, i1, i2));
                    m_triangles.push_back(Triangle(i3, i2, i1));
                }
            }
        }
    }
    centerAndScaleToUnit ();
    recomputeNormals ();
}

void Mesh::recomputeNormals () {
    m_normals.clear ();
    m_normals.resize (m_positions.size (), Vec3f (0.f, 0.f, 0.f));
    for (unsigned int i = 0; i < m_triangles.size (); i++) {
        Vec3f e01 = m_positions[m_triangles[i][1]] -  m_positions[m_triangles[i][0]];
        Vec3f e02 = m_positions[m_triangles[i][2]] -  m_positions[m_triangles[i][0]];
        Vec3f n = cross (e01, e02);
        n.normalize ();
        for (unsigned int j = 0; j < 3; j++)
            m_normals[m_triangles[i][j]] += n;
    }
    for (unsigned int i = 0; i < m_normals.size (); i++)
        m_normals[i].normalize ();
}

void Mesh::centerAndScaleToUnit () {
    Vec3f c;
    for  (unsigned int i = 0; i < m_positions.size (); i++)
        c += m_positions[i];
    c /= m_positions.size ();
    float maxD = dist (m_positions[0], c);
    for (unsigned int i = 0; i < m_positions.size (); i++){
        float m = dist (m_positions[i], c);
        if (m > maxD)
            maxD = m;
    }
    for  (unsigned int i = 0; i < m_positions.size (); i++)
        m_positions[i] = (m_positions[i] - c) / maxD;
}
