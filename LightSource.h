#pragma once

#include"Vec3.h"

class LightSource {
public :
    LightSource();
	LightSource(float x, float y, float z);
  void setPosition(float x, float y, float z);
  void setColor(float r, float g, float b);
  Vec3f getPosition();
  Vec3f getColor();
//	void setPosition(float x1, float y1, float z1);

private :
    Vec3f position;
    Vec3f color;
};
