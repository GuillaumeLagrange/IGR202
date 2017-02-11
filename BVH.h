#pragma once

#include <iostream>

#include "Vec3.h"
#include "Mesh.h"
#include "BoundingBox.h"

class BVH {
    private :
        const Mesh * mesh;

        std::vector<int> tri_index;
        BoundingBox bBox;
        BVH * leftChild;
        BVH * rightChild;

        /* Stopping criteria */
        static unsigned int max_density;
        static unsigned int nodes;
        static unsigned int leaves;

        void split(std::vector<int> &_child_tri_index_1,
                   std::vector<int> &_child_tri_index_2,
                   BoundingBox &child_box_1, BoundingBox &child_box_2);

    public :
        ~BVH() {};
        /* Constructors */
        BVH();
        BVH(const Mesh &mesh);
        BVH(const Mesh &mesh, const std::vector<int> &tri_index,
                const BoundingBox &_bBox);

        /* Getters */
        const BVH*  getLeftChild() {return leftChild;}
        const BVH*  getRightChild() {return rightChild;}
        const BoundingBox getBBox() {return bBox;}
        const std::vector<int> getIndexes() const {return tri_index;}
        const Mesh* getMesh() const {return mesh;}

        const void draw(std::vector<float> &colors) {
            if (leftChild != NULL)
                leftChild->draw(colors);
            if (rightChild != NULL)
                rightChild->draw(colors);
            if (leftChild == NULL && rightChild == NULL)
                bBox.draw(mesh, colors, tri_index);
        }
};
