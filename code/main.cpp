#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <optional>
#include <random>
#include <string>
#include <vector>
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif
#define RAYGUI_IMPLEMENTATION 1
#include <raylib/raygui.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>

#include "Grid.h"
#include "Scene.h"

Camera3D g_camera;
Grid g_grid = { .Slices = 10, .Spacing = 1.0f };
Scene g_scene;
Color g_object_color = RAYWHITE;
int g_current_object_type_index = (int)ObjectType::CUBE;
Model g_current_model;

void UpdateAndDrawFrame()
{
#if !__EMSCRIPTEN__
    // https://en.wikipedia.org/wiki/Table_of_keyboard_shortcuts
    if (IsKeyPressed(KEY_F11))
    {
        ToggleFullscreen();
    }
#endif

    // UPDATE.
    UpdateCamera(&g_camera);

    // CHECK FOR OBJECT COLLISIONS.
    const Object3D* selected_object = nullptr;
    {
        Vector2 mouse_position = GetMousePosition();
        Ray mouse_ray = GetMouseRay(mouse_position, g_camera);

        for (const auto& object_3D : g_scene.Objects)
        {
            BoundingBox object_bounding_box = object_3D.GetBoundingBox();
            bool collision = CheckCollisionRayBox(mouse_ray, object_bounding_box);
            if (collision)
            {
                selected_object = &object_3D;
                break;
            }
        }
    }

    // ADJUST THE THIRD DIMENSION FOR PLACING OBJECTS.
    std::optional<Ray> ray_to_view;
    static float ground_height = 0.0f;

    int mouse_wheel_movement = GetMouseWheelMove();
    if (mouse_wheel_movement)
    {
        std::printf("Mouse wheel movement: %d\n", mouse_wheel_movement);
    }
    //ground_height -= (float)mouse_wheel_movement;

    if (IsKeyPressed(KEY_ONE))
    {
        ground_height -= 1.0f;
    }
    if (IsKeyPressed(KEY_TWO))
    {
        ground_height += 1.0f;
    }

    // DETERMINE WHERE THE MOUSE CURRENTLY POINTS IN 3D SPACE.
    std::random_device random_number_generator;

    Vector2 mouse_position = GetMousePosition();
    Ray mouse_ray = GetMouseRay(mouse_position, g_camera);
    ray_to_view = mouse_ray;

    Vector3 mouse_click_position_3D;
    float grid_size = (g_grid.Slices * g_grid.Spacing);
    int grid_size_rounded_up = (int)std::ceilf(grid_size);
    int grid_half_size = (int)(grid_size / 2.0f);
    unsigned int random_x = random_number_generator();
    unsigned int random_y = random_number_generator();
    unsigned int random_z = random_number_generator();
    int random_x_in_grid = (int)(random_x % grid_size_rounded_up);
    int random_y_in_grid = (int)(random_y % grid_size_rounded_up);
    int random_z_in_grid = (int)(random_z % grid_size_rounded_up);
    mouse_click_position_3D.x = (float)(random_x_in_grid - grid_half_size);
    mouse_click_position_3D.y = (float)(random_y_in_grid - grid_half_size);
    mouse_click_position_3D.z = (float)(random_z_in_grid - grid_half_size);

    Vector3 up_vector = { 0.0f, 1.0f, 0.0f };
    float scale = Vector3DotProduct(mouse_ray.direction, up_vector);

    RayHitInfo ray_hit_info = GetCollisionRayGround(mouse_ray, -scale * ground_height);
    //RayHitInfo ray_hit_info = GetCollisionRayGround(mouse_ray, ground_height);
    mouse_click_position_3D = ray_hit_info.position;

    // PLACE AN OBJECT IF THE USER CLICKED.
    /// @todo   Prevent this if GUI being interacted with!
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        std::printf("Creating object at %f, %f, %f\n", mouse_click_position_3D.x, mouse_click_position_3D.y, mouse_click_position_3D.z);

        ObjectType current_object_type = (ObjectType)g_current_object_type_index;
        switch (current_object_type)
        {
            case ObjectType::LINE:
            {
                Object3D line =
                {
                    .Type = ObjectType::LINE,
                    .SolidColor = g_object_color,
                    .Line =
                    {
                        .StartPosition = mouse_click_position_3D,
                        .EndPosition = Vector3Add(mouse_click_position_3D, Vector3 {1.0f, 1.0f, 1.0f})
                    }
                };
                g_scene.Objects.emplace_back(line);
                break;
            }
            case ObjectType::CUBE:
            {
                Object3D cube =
                {
                    .Type = ObjectType::CUBE,
                    .SolidColor = g_object_color,
                    .Cube =
                    {
                        .CenterPosition = mouse_click_position_3D,
                        .Size = Vector3{ 1.0f, 1.0f, 1.0f }
                    }
                };
                g_scene.Objects.emplace_back(cube);
                break;
            }
            case ObjectType::SPHERE:
            {
                Object3D sphere =
                {
                    .Type = ObjectType::SPHERE,
                    .SolidColor = g_object_color,
                    .Sphere =
                    {
                        .CenterPosition = mouse_click_position_3D,
                        .Radius = 0.5f
                    }
                };
                g_scene.Objects.emplace_back(sphere);
                break;
            }
            case ObjectType::CYLINDER:
            {
                Object3D cylinder =
                {
                    .Type = ObjectType::CYLINDER,
                    .SolidColor = g_object_color,
                    .Cylinder =
                    {
                        .CenterPosition = mouse_click_position_3D,
                        .TopRadius = 0.5f,
                        .BottomRadius = 1.0f,
                        .Height = 1.0f,
                        .SliceCount = 16,
                    }
                };
                g_scene.Objects.emplace_back(cylinder);
                break;
            }
            case ObjectType::MODEL:
            {
                Object3D model =
                {
                    .Type = ObjectType::MODEL,
                    .SolidColor = g_object_color,
                    .Model = 
                    {
                        .CenterPosition = mouse_click_position_3D,
                        .Model = g_current_model
                    }
                };
                g_scene.Objects.emplace_back(model);
                break;
            }
        }
    }

    // DRAW.
    BeginDrawing();
    ClearBackground(g_scene.BackgroundColor);

    BeginMode3D(g_camera);
    {
        // DRAW THE GRID.
        DrawGrid((int)g_grid.Slices, g_grid.Spacing);

        // DRAW EACH OBJECT IN THE SCENE.
        for (const auto& object_3D : g_scene.Objects)
        {
            Color object_color = object_3D.SolidColor;
            bool current_object_selected = (selected_object == &object_3D);
            if (current_object_selected)
            {
                constexpr float COLOR_BRIGHTNESS_FACTOR = 1.3f;
                object_color.r *= COLOR_BRIGHTNESS_FACTOR;
                object_color.g *= COLOR_BRIGHTNESS_FACTOR;
                object_color.b *= COLOR_BRIGHTNESS_FACTOR;
            }

            switch (object_3D.Type)
            {
                case ObjectType::LINE:
                {
                    DrawLine3D(object_3D.Line.StartPosition, object_3D.Line.EndPosition, object_color);
                    break;
                }
                case ObjectType::CUBE:
                {
                    DrawCubeV(object_3D.Cube.CenterPosition, object_3D.Cube.Size, object_3D.SolidColor);
                    if (current_object_selected)
                    {
                        DrawCubeWiresV(object_3D.Cube.CenterPosition, object_3D.Cube.Size, object_color);
                    }
                    break;
                }
                case ObjectType::SPHERE:
                {
                    DrawSphere(object_3D.Sphere.CenterPosition, object_3D.Sphere.Radius, object_3D.SolidColor);
                    if (current_object_selected)
                    {
                        constexpr int RING_COUNT = 12;
                        constexpr int SLICE_COUNT = 12;
                        DrawSphereWires(object_3D.Sphere.CenterPosition, object_3D.Sphere.Radius, RING_COUNT, SLICE_COUNT, object_color);
                    }
                    break;
                }
                case ObjectType::CYLINDER:
                {
                    DrawCylinder(
                        object_3D.Cylinder.CenterPosition,
                        object_3D.Cylinder.TopRadius,
                        object_3D.Cylinder.BottomRadius,
                        object_3D.Cylinder.Height,
                        object_3D.Cylinder.SliceCount,
                        object_3D.SolidColor);
                    if (current_object_selected)
                    {
                        DrawCylinderWires(
                            object_3D.Cylinder.CenterPosition,
                            object_3D.Cylinder.TopRadius,
                            object_3D.Cylinder.BottomRadius,
                            object_3D.Cylinder.Height,
                            object_3D.Cylinder.SliceCount,
                            object_color);
                    }
                    break;
                }
                case ObjectType::MODEL:
                {
                    DrawModel(object_3D.Model.Model, object_3D.Model.CenterPosition, 1.0f, object_3D.SolidColor);
                    if (current_object_selected)
                    {
                        DrawModel(object_3D.Model.Model, object_3D.Model.CenterPosition, 1.0f, object_3D.SolidColor);
                    }
                    break;
                };
            }
        }

        // DRAW AN ITEM FOR THE CURRENT MOUSE LOCATION.
        ObjectType current_object_type = (ObjectType)g_current_object_type_index;
        switch (current_object_type)
        {
            case ObjectType::SPHERE:
            {
                constexpr int RING_COUNT = 12;
                constexpr int SLICE_COUNT = 12;
                DrawSphereWires(ray_hit_info.position, 0.5f, RING_COUNT, SLICE_COUNT, g_object_color);
                break;
            }
            case ObjectType::CYLINDER:
            {
                DrawCylinderWires(
                    ray_hit_info.position,
                    0.5f,
                    1.0f,
                    1.0f,
                    16,
                    g_object_color);
                break;
            }
            default:
            {
                DrawCubeWires(
                    ray_hit_info.position,
                    1.0f,
                    1.0f,
                    1.0f,
                    g_object_color);
                break;
            }
        }
    }
    EndMode3D();

    // DRAW THE GUI.
    Rectangle grid_slices_slider_bounding_box = { .x = 0, .y = 30, .width = 100, .height = 20 };
    g_grid.Slices = GuiSlider(
        grid_slices_slider_bounding_box,
        "", 
        "Grid Slices",
        g_grid.Slices, 
        1.0f, 
        100.0f);

    Rectangle grid_spacing_slider_bounding_box = grid_slices_slider_bounding_box;
    grid_spacing_slider_bounding_box.y += grid_slices_slider_bounding_box.height;
    g_grid.Spacing = GuiSlider(
        grid_spacing_slider_bounding_box,
        "", 
        "Grid Spacing",
        g_grid.Spacing, 
        1.0f, 
        100.0f);

    Rectangle color_picker_bounding_box = grid_spacing_slider_bounding_box;
    color_picker_bounding_box.y += grid_spacing_slider_bounding_box.height;
    color_picker_bounding_box.height = 100.f;
    g_object_color = GuiColorPicker(color_picker_bounding_box, g_object_color);

    constexpr int MAX_MODEL_FILEPATH = 256;
    static char model_filepath[MAX_MODEL_FILEPATH] = {};
    Rectangle model_filename_bounding_box = color_picker_bounding_box;
    model_filename_bounding_box.y += color_picker_bounding_box.height;
    model_filename_bounding_box.width = 600;
    model_filename_bounding_box.height = grid_spacing_slider_bounding_box.height;
    GuiTextBox(model_filename_bounding_box, model_filepath, MAX_MODEL_FILEPATH, true);

    static bool object_type_drop_down_open = false;
    Rectangle object_type_drop_down_bounds = model_filename_bounding_box;
    object_type_drop_down_bounds.width = color_picker_bounding_box.height;
    object_type_drop_down_bounds.y += model_filename_bounding_box.height;
    bool drop_down_clicked = GuiDropdownBox(
        object_type_drop_down_bounds, 
        "Line\nCube\nSphere\nCylinder\nModel", 
        &g_current_object_type_index, 
        object_type_drop_down_open);
    if (drop_down_clicked)
    {
        object_type_drop_down_open = !object_type_drop_down_open;

        if (ObjectType::MODEL == (ObjectType)g_current_object_type_index)
        {
            g_current_model = LoadModel(model_filepath);
        }
    }
    
    // DRAW THE FRAME RATE.
    DrawFPS(10, 10);

    std::string ground_height_text = "Ground Height: " + std::to_string(ground_height);
    DrawText(ground_height_text.c_str(), 100, 10, 10, LIME);

    EndDrawing();
}

int main()
{
    // GENERIC INIT.
    constexpr int SCREEN_WIDTH_IN_PIXELS = 800;
    constexpr int SCREEN_HEIGHT_IN_PIXELS = 600;
    InitWindow(SCREEN_WIDTH_IN_PIXELS, SCREEN_HEIGHT_IN_PIXELS, "raylib");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // SPECIFIC INIT.
    g_camera.position = { 0.0f, 5.0f, 10.0f };
    g_camera.target = { 0.0f, 0.0f, 0.0f };
    g_camera.up = { 0.0f, 1.0f, 0.0f };
    g_camera.fovy = 45.0f;
    g_camera.type = CAMERA_PERSPECTIVE;

    SetCameraMode(g_camera, CAMERA_FREE);

#if __EMSCRIPTEN__
    constexpr int LET_BROWSER_CONTROL_FRAME_RATE = 0;
    constexpr int SIMULATE_INFINITE_LOOP = 1;
    emscripten_set_main_loop(UpdateAndDrawFrame, LET_BROWSER_CONTROL_FRAME_RATE, SIMULATE_INFINITE_LOOP);
#else
    SetTargetFPS(60);
    while (!WindowShouldClose())
    {
        // No need to check if window is minimized.
        // This won't get called if the window is minimized.
        UpdateAndDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}