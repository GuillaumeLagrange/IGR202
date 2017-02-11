#include "BVH.h"
#include "Triangle.h"
#include <cfloat>

unsigned int BVH::max_density = 500;
unsigned int BVH::nodes = 0;
unsigned int BVH::leaves = 0;

BVH::BVH() : mesh(NULL) {}

BVH::BVH(const Mesh &_mesh) : mesh(&_mesh) {
    std::vector<Vec3f> positions = mesh->positions();
    std::vector<Triangle> triangles = mesh->triangles();

    std::vector<int> t(triangles.size());
    for (unsigned int i = 0; i < triangles.size(); i++) {
        t[i] = i;
    }

    tri_index = t;

    float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
    float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;
    Vec3f meanPos = Vec3f(0.0,0.0,0.0);

    for (unsigned int i = 0; i < positions.size(); i++) {
        Vec3f currentPos = positions[i];

        meanPos += currentPos;

        /* Updating maximal values */
        if (currentPos[0] < minX)
            minX = currentPos[0];
        if (currentPos[1] < minY)
            minY = currentPos[1];
        if (currentPos[2] < minZ)
            minZ = currentPos[2];
        if (currentPos[0] > maxX)
            maxX = currentPos[0];
        if (currentPos[1] > maxY)
            maxY = currentPos[1];
        if (currentPos[0] > maxZ)
            maxZ = currentPos[2];
    }

    meanPos *= 1.f/(float) positions.size();

    Vec3f lowPos = Vec3f(minX, minY, minZ);
    Vec3f uppPos = Vec3f(maxX, maxY, maxZ);

    bBox = BoundingBox(lowPos, uppPos, meanPos);

    std::vector<int> child_tri_index_1;
    std::vector<int> child_tri_index_2;
    BoundingBox child_box_1;
    BoundingBox child_box_2;

    split(child_tri_index_1, child_tri_index_2, child_box_1, child_box_2);

    leftChild = new BVH(_mesh, child_tri_index_1, child_box_1);
    rightChild = new BVH(_mesh, child_tri_index_2, child_box_2);

    nodes ++;
}

BVH::BVH(const Mesh &_mesh, const std::vector<int> &_tri_index,
        const BoundingBox &_bBox) :
    mesh(&_mesh), tri_index(_tri_index), bBox(_bBox), leftChild(NULL),
    rightChild(NULL) {
        if (tri_index.size() > max_density) {
            std::vector<int> child_tri_index_1;
            std::vector<int> child_tri_index_2;
            BoundingBox child_box_1;
            BoundingBox child_box_2;

            split(child_tri_index_1, child_tri_index_2,
                    child_box_1, child_box_2);

            leftChild = new BVH(_mesh, child_tri_index_1, child_box_1);
            rightChild = new BVH(_mesh, child_tri_index_2, child_box_2);

            nodes ++;
        }
    }

void BVH::split(std::vector<int> &child_tri_index_1,
        std::vector<int> &child_tri_index_2,
        BoundingBox &child_box_1, BoundingBox &child_box_2) {

    std::vector<Vec3f> positions = mesh->positions();
    std::vector<Triangle> triangles = mesh->triangles();

    /* Chosing largest dimension */
    float x = bBox.uppCorner[0] - bBox.lowCorner[0];
    float y = bBox.uppCorner[1] - bBox.lowCorner[1];
    float z = bBox.uppCorner[2] - bBox.lowCorner[2];

    int splt;
    if(x>=y && x >=z)
        splt = 0;
    else if(y >= z)
        splt = 1;
    else
        splt = 2;

    /* Splitting triangles between the chidlren */
    float maxX1 = FLT_MIN, maxY1 = FLT_MIN, maxZ1 = FLT_MIN;
    float minX1 = FLT_MAX, minY1 = FLT_MAX, minZ1 = FLT_MAX;

    float maxX2 = FLT_MIN, maxY2 = FLT_MIN, maxZ2 = FLT_MIN;
    float minX2 = FLT_MAX, minY2 = FLT_MAX, minZ2 = FLT_MAX;

    Vec3f meanPos1 = Vec3f(0,0,0);
    Vec3f meanPos2 = Vec3f(0,0,0);

    Vec3f meanPos = bBox.meanPos;

    for(unsigned int i = 0; i < tri_index.size(); i++) {
        Triangle currentTri = triangles[tri_index[i]];
        Vec3f p0 = positions[currentTri[0]];
        Vec3f p1 = positions[currentTri[1]];
        Vec3f p2 = positions[currentTri[2]];
        Vec3f barycenter = (p0 + p1 + p2) / 3.f;

        if(barycenter[splt] > meanPos[splt]) {
            child_tri_index_1.push_back(tri_index[i]);
            meanPos1 += barycenter;

            for(int k = 0; k < 3; k++) {
                float x1 = positions[currentTri[k]][0];
                float y1 = positions[currentTri[k]][1];
                float z1 = positions[currentTri[k]][2];

                if(x1 < minX1)
                    minX1 = x1;
                if(y1 < minY1)
                    minY1 = y1;
                if(z1 < minZ1)
                    minZ1 = z1;

                if(x1 > maxX1)
                    maxX1 = x1;
                if(y1 > maxY1)
                    maxY1 = y1;
                if(z1 > maxZ1)
                    maxZ1 = z1;
            }
        } else {
            child_tri_index_2.push_back(tri_index[i]);
            meanPos2 += barycenter;

            for(int k = 0; k < 3; k++) {
                float x2 = positions[currentTri[k]][0];
                float y2 = positions[currentTri[k]][1];
                float z2 = positions[currentTri[k]][2];

                if(x2 < minX2)
                    minX2 = x2;
                if(y2 < minY2)
                    minY2 = y2;
                if(z2 < minZ2)
                    minZ2 = z2;

                if(x2 > maxX2)
                    maxX2 = x2;
                if(y2 > maxY2)
                    maxY2 = y2;
                if(z2 > maxZ2)
                    maxZ2 = z2;
            }
        }
    }

    meanPos1 *= 1.0/(float) child_tri_index_1.size();
    meanPos2 *= 1.0/(float) child_tri_index_2.size();

    Vec3f lowPos1 = Vec3f(minX1, minY1, minZ1);
    Vec3f uppPos1 = Vec3f(maxX1, maxY1, maxZ1);
    child_box_1 = BoundingBox(lowPos1, uppPos1, meanPos1);

    Vec3f lowPos2 = Vec3f(minX2, minY2, minZ2);
    Vec3f uppPos2 = Vec3f(maxX2, maxY2, maxZ2);
    child_box_2 = BoundingBox(lowPos2, uppPos2, meanPos2);
}
