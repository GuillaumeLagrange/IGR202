#pragma once

#include "Mesh.h"
#include "Triangle.h"
#include "Vec3.h"
#include <cfloat>

class BoundingBox {

    private :
        /* Corners */
        Vec3f lowCorner;
        Vec3f uppCorner;

        /* Means */
        Vec3f meanPos;
        Vec3f meanNor;
        Vec3f meanCol;

        /* Indexes of triangles inside the bounding box */
        std::vector<int> triIndex;

    public :
        BoundingBox() {}

        BoundingBox(const Mesh &mesh, std::vector<int> _triIndex) :       triIndex(_triIndex) {
            meanNor = Vec3f(0.0,0.0,0.0);
            meanPos = Vec3f(0.0,0.0,0.0);

            std::vector<Triangle> triangles = mesh.triangles();
            std::vector<Vec3f> positions = mesh.positions();
            std::vector<Vec3f> normals = mesh.normals();

            float maxX = FLT_MIN, maxY = FLT_MIN, maxZ = FLT_MIN;
            float minX = FLT_MAX, minY = FLT_MAX, minZ = FLT_MAX;

            for (unsigned int i; i < triIndex.size(); i++) {
                int j = triIndex[i];
                Triangle currentTri = triangles[j];

                for (int k = 0; k < 3; k++) {
                    Vec3f currentPos = positions[currentTri[k]];
                    Vec3f currentNor = positions[currentTri[k]];

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

                    /* Updating mean values */
                    meanPos += currentPos / 3.f;
                    meanNor += currentNor / 3.f;
                }
            }

            uppCorner = Vec3f(maxX, maxY, maxZ);
            lowCorner = Vec3f(minX, minY, minZ);

            meanPos *= 1.f/(float) triIndex.size();
        }

        /* Setters */
		void setLowCorner(const Vec3f &_lowCorner) {lowCorner = _lowCorner;}
		void setUppCorner(const Vec3f &_uppCorner) {uppCorner = _uppCorner;}

        /* Getters */
		const Vec3f getLowCorner() {return lowCorner;}
		const Vec3f getUppCorner() {return uppCorner;}
		const Vec3f getMeanPos() {return meanPos;}
		const Vec3f getMeanNor() {return meanNor;}
		const int getDensity() {return trianglesIndexes.size();}
		const std::vector<int> getTriIndex() {return triIndex;}


		const int getLargestDim() {
			float x, y, z;
			x = uppCorner[0] - lowCorner[0];
			y = uppCorner[1] - lowCorner[1];
			z = uppCorner[2] - lowCorner[2];

			if((x >= y) && (x >= z))
                return 0;
			else if(y >= z)
                return 1;
			return 2;
		}
		const bool isEmpty() { return triIndex.empty(); }

        const std::vector<BoundingBox> split(const Mesh &mesh) {
            int i = getLargestDim();
        }
};
