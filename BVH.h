#pragma once

#include <iostream>

#include "Vec3.h"
#include "Mesh.h"
#include "BoundingBox.h"

class BVH {
    private :
        BoundingBox bBox;
        BVH * leftChild;
        BVH * rightChild;
        /* Stopping criteria */
        static int max_density;
        static int max_nodes;

    public :
        BVH() {}

        /* Constructor with max_nodes stopping */
        BVH(const Mesh &mesh) {
            std::vector<int> triIndex;
            std::vector<Triangle> triangles = mesh.triangles();

            for (unsigned int i = 0; i < triangles.size(); i++)
                triIndex.push_back(i);

            bBox = BoundingBox(mesh, triIndex);
            std::vector<BoundingBox> children = bBox.split(mesh);
            leftChild = new BVH(mesh, children[0]);
            rightChild = new BVH(mesh, children[1]);
            max_nodes --;
        }

        /* Constructor with max_density stopping */
        BVH(const Mesh &mesh, const BoundingBox &_bBox) :
            bBox(_bBox), leftChild(NULL), rightChild(NULL) {
                if(bBox.getDensity() > max_density) {
                    std::vector<BoundingBox> children = bBox.split(mesh);
                    leftChild = new BVH(mesh, children[0]);
                    rightChild = new BVH(mesh, children[1]);
                }
        }

        /* Getters */
        const Vec3f getMeanPos() {return bBox.getMeanPos();}
        const Vec3f getMeanNor() {return bBox.getMeanNor();}

        const void draw(const Mesh &mesh, std::vector<float> &colors) {
            if (leftChild != NULL)
                leftChild->draw(mesh, colors);
            if (rightChild != NULL)
                rightChild->draw(mesh, colors);
            if (leftChild == NULL && rightChild == NULL)
                bBox.draw(mesh, colors);
        }
};

int BVH::max_density = 100;
int BVH::max_nodes = 100;
