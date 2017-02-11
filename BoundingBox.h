#pragma once

#include "Vec3.h"

class BoundingBox {
    public:
        Vec3f meanPos;
        Vec3f lowCorner;
        Vec3f uppCorner;
        Vec3f color;

        ~BoundingBox() {}

        BoundingBox() {
            meanPos = Vec3f(0.0,0.0,0.0);
            lowCorner = Vec3f(0.0,0.0,0.0);
            uppCorner = Vec3f(0.0,0.0,0.0);
            color = Vec3f(0.0,0.0,0.0);
        }

        BoundingBox(Vec3f _meanPos, Vec3f _lowCorner, Vec3f _uppCorner) :
            meanPos(_meanPos), lowCorner(_lowCorner), uppCorner(_uppCorner),
            color(Vec3f(0.0,0.0,0.0)) {}

        const void draw (const Mesh * &mesh, std::vector<float> &colors,
                         std::vector<int> &tri_index) {
            std::vector<Triangle> triangles = mesh->triangles();
            std::vector<Vec3f> positions = mesh->positions();

            Vec3f randColor = Vec3f(rand()%255/255,
                                    rand()%255/255,
                                    rand()%255/255);

            for (unsigned int i = 0; i< tri_index.size(); i++) {
                int j = tri_index[i];
                Triangle currentTri = triangles[j];

                colors[4*currentTri[0]    ] = randColor[0];
                colors[4*currentTri[0] + 1] = randColor[1];
                colors[4*currentTri[0] + 2] = randColor[2];

                colors[4*currentTri[1]    ] = randColor[0];
                colors[4*currentTri[1] + 1] = randColor[1];
                colors[4*currentTri[1] + 2] = randColor[2];

                colors[4*currentTri[2]    ] = randColor[0];
                colors[4*currentTri[2] + 1] = randColor[1];
                colors[4*currentTri[2] + 2] = randColor[2];
            }
        }
};
