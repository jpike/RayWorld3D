#pragma once

#include <algorithm>
#include <raylib/raylib.h>
#include <raylib/raymath.h>
#include "Cube.h"
#include "Cylinder.h"
#include "Line.h"
#include "Model3D.h"
#include "ObjectType.h"
#include "Sphere.h"

class Object3D
{
public:
    BoundingBox GetBoundingBox() const
    {
        BoundingBox bounding_box;

        switch (Type)
        {
            case ObjectType::LINE:
            {
                bounding_box.min = Vector3Min(Line.StartPosition, Line.EndPosition);
                bounding_box.max = Vector3Max(Line.StartPosition, Line.EndPosition);
                break;
            }
            case ObjectType::CUBE:
            {
                Vector3 cube_half_size = Vector3Scale(Cube.Size, 0.5f);
                bounding_box.min = Vector3Subtract(Cube.CenterPosition, cube_half_size);
                bounding_box.max = Vector3Add(Cube.CenterPosition, cube_half_size);
                break;
            }
            case ObjectType::SPHERE:
            {
                bounding_box.min = Vector3SubtractValue(Sphere.CenterPosition, Sphere.Radius);
                bounding_box.max = Vector3AddValue(Sphere.CenterPosition, Sphere.Radius);
                break;
            }
            case ObjectType::CYLINDER:
            {
                float cylinder_max_radius = std::max(Cylinder.TopRadius, Cylinder.TopRadius);
                float cylinder_half_height = Cylinder.Height / 2.0f;
                Vector3 cylinder_half_size = { cylinder_max_radius, cylinder_half_height, cylinder_max_radius };

                bounding_box.min = Vector3Subtract(Cylinder.CenterPosition, cylinder_half_size);
                bounding_box.max = Vector3Add(Cylinder.CenterPosition, cylinder_half_size);
                break;
            }
        }

        return bounding_box;
    }

    ObjectType Type = ObjectType::LINE;
    Color SolidColor = BLACK;

    union
    {
        Line Line;
        Cube Cube;
        Sphere Sphere;
        Cylinder Cylinder;
        Model3D Model;
    };
};
