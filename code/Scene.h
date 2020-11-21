#pragma once

#include <vector>
#include <raylib/raylib.h>
#include "Object3D.h"

class Scene
{
public:
    Color BackgroundColor = BLACK;
    std::vector<Object3D> Objects = {};
};
