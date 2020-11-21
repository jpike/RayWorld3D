#pragma once

#include <raylib/raylib.h>

class Cylinder
{
public:
    Vector3 CenterPosition = {};
    float TopRadius = 0.0f;
    float BottomRadius = 0.0f;
    float Height = 0.0f;
    int SliceCount = 0;
};
