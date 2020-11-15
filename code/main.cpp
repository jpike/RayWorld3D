#include <algorithm>
#include <cstdint>
#include <cstdio>
#include <limits>
#include <optional>
#include <random>
#include <vector>
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif
#define RAYGUI_IMPLEMENTATION 1
#include <raylib/raygui.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>

Camera3D g_camera;

enum class ObjectType
{
    INVALID = 0,
    LINE,
    CUBE,
    SPHERE,
    CYLINDER,
};

class Line
{
public:
    Vector3 StartPosition = {};
    Vector3 EndPosition = {};
};

class Cube
{
public:
    Vector3 CenterPosition = {};
    Vector3 Size = {};
};

class Sphere
{
public:
    Vector3 CenterPosition = {};
    float Radius = 0.0f;
};

class Cylinder
{
public:
    Vector3 CenterPosition = {};
    float TopRadius = 0.0f;
    float BottomRadius = 0.0f;
    float Height = 0.0f;
    int SliceCount = 0;
};

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

    ObjectType Type = ObjectType::INVALID;
    Color SolidColor = BLACK;

    union
    {
        Line Line;
        Cube Cube;
        Sphere Sphere;
        Cylinder Cylinder;
    };
};

class Scene
{
public:
    Color BackgroundColor = BLACK;
    std::vector<Object3D> Objects = {};
};

Scene g_scene;

void UpdateAndDrawFrameGui()
{
    // UPDATE.
    UpdateCamera(&g_camera);

    // DRAW.
    BeginDrawing();
    ClearBackground(g_scene.BackgroundColor);

    BeginMode3D(g_camera);
    {
    }
    EndMode3D();

    static bool gui_window_open = true;
    static Vector2 scroll_position = { 0.0f, 0.0f };
    if (gui_window_open)
    {
        gui_window_open = !GuiWindowBox({ .x = 10, .y = 20, .width = 550, .height = 350 }, "GuiWindowBox");
        GuiGroupBox({ .x = 15, .y = 60, .width = 100, .height = 20 }, "GuiGroupBox");
        GuiLine({ .x = 15, .y = 90, .width = 100, .height = 20 }, "GuiLine");
        GuiPanel({ .x = 15, .y = 110, .width = 100, .height = 20});
        GuiScrollPanel(
            { .x = 15, .y = 140, .width = 100, .height = 50 },
            { .x = 17, .y = 142, .width = 200, .height = 100 },
            &scroll_position);

        GuiLabel({ .x = 15, .y = 200, .width = 100, .height = 20 }, "GuiLabel");
        GuiButton({ .x = 15, .y = 240, .width = 100, .height = 20 }, "GuiButton");
        GuiLabelButton({ .x = 15, .y = 270, .width = 100, .height = 20 }, "GuiLabelButton");

        GuiToggle({ .x = 120, .y = 60, .width = 100, .height = 20 }, "GuiToggle", true);
        GuiToggleGroup({ .x = 120, .y = 90, .width = 100, .height = 20 }, "Gui\nToggle\nGroup", 0);

        GuiCheckBox({ .x = 120, .y = 150, .width = 100, .height = 20 }, "GuiCheckBox", true);
        GuiComboBox({ .x = 120, .y = 180, .width = 100, .height = 20 }, "Gui\nCombo\nBox", 0);
        int active = 0;
        GuiDropdownBox({ .x = 120, .y = 210, .width = 100, .height = 20 }, "Gui\nDropdown\nBox", &active, false);

        static int spinner_value = 1;
        GuiSpinner(
            { .x = 250, .y = 60, .width = 100, .height = 20 },
            "GuiSpinner",
            &spinner_value,
            1,
            100,
            false);

        static int value_box_value = 2;
        GuiValueBox(
            { .x = 250, .y = 90, .width = 100, .height = 20 },
            "GuiValueBox",
            & value_box_value,
            1,
            100,
            false);

        char text_box_text[] = "GuiTextBox";
        GuiTextBox(
            { .x = 250, .y = 120, .width = 100, .height = 20 }, 
            text_box_text, 
            sizeof(text_box_text),
            false);

        char multi_text_box_text[] = "GuiTextBoxMulti";
        GuiTextBoxMulti(
            { .x = 250, .y = 150, .width = 100, .height = 20 },
            multi_text_box_text,
            sizeof(multi_text_box_text),
            false);

        GuiSlider(
            { .x = 250, .y = 180, .width = 100, .height = 20 },
            "GuiSliderTextLeft", 
            "GuiSliderTextRight",
            2.0f, 
            1.0f, 
            10.0f);

        GuiSliderBar(
            { .x = 250, .y = 210, .width = 100, .height = 20 },
            "GuiSliderBarTextLeft",
            "GuiSliderBarTextRight",
            2.0f,
            1.0f,
            10.0f);

        GuiProgressBar(
            { .x = 250, .y = 240, .width = 100, .height = 20 },
            "GuiProgressBarTextLeft",
            "GuiProgressBarTextRight",
            2.0f,
            1.0f,
            10.0f);

        GuiStatusBar({ .x = 250, .y = 270, .width = 100, .height = 20 }, "GuiStatusBar");

        GuiScrollBar(
            { .x = 370, .y = 60, .width = 50, .height = 20 },
            2, 
            1, 
            5);

        GuiGrid({ .x = 370, .y = 90, .width = 50, .height = 20 }, 4.0f, 2);

        static int scroll_index = 0;
        GuiListView(
            { .x = 420, .y = 120, .width = 50, .height = 20 }, 
            "Gui\nList\nView", 
            &scroll_index, 
            false);
        //GuiListViewEx(Rectangle bounds, const char** text, int count, int* focus, int* scrollIndex, int active);
        GuiMessageBox(
            { .x = 470, .y = 150, .width = 50, .height = 20 },
            "GuiMessageBoxTitle", 
            "GuiMessageBoxMessage", 
            "OK\nCancel");
        char text_input[] = "text input";
        GuiTextInputBox(
            { .x = 470, .y = 180, .width = 50, .height = 20 },
            "GuiTextInputBoxTitle", 
            "GuiTextInputBoxMessage", 
            "OK\nCancel",
            text_input);

        GuiColorPicker({ .x = 420, .y = 210, .width = 50, .height = 20 }, BLACK);
        GuiColorPanel({ .x = 420, .y = 240, .width = 50, .height = 20 }, RED);
        GuiColorBarAlpha({ .x = 420, .y = 270, .width = 50, .height = 20 }, 1.0f);
        GuiColorBarHue({ .x = 420, .y = 300, .width = 50, .height = 20 }, 0.5f);
    }

    EndDrawing();
}

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

    std::optional<Ray> ray_to_view;
    static float ground_height = 0.0f;

    int mouse_wheel_movement = GetMouseWheelMove();
    if (mouse_wheel_movement)
    {
        std::printf("Mouse wheel movement: %d\n", mouse_wheel_movement);
    }
    //ground_height -= (float)mouse_wheel_movement;

    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
    {
        std::random_device random_number_generator;

        Vector2 mouse_position = GetMousePosition();
        Ray mouse_ray = GetMouseRay(mouse_position, g_camera);
        ray_to_view = mouse_ray;

        Vector3 mouse_click_position_3D;
        /// @todo   Proper camera unprojecting to calculate position.
        constexpr unsigned int GRID_SIZE = 10;
        constexpr int GRID_HALF_SIZE = static_cast<int>(GRID_SIZE / 2);
        unsigned int random_x = random_number_generator();
        unsigned int random_y = random_number_generator();
        unsigned int random_z = random_number_generator();
        int random_x_in_grid = static_cast<int>(random_x % GRID_SIZE);
        int random_y_in_grid = static_cast<int>(random_y % GRID_SIZE);
        int random_z_in_grid = static_cast<int>(random_z % GRID_SIZE);
        mouse_click_position_3D.x = static_cast<float>(random_x_in_grid - GRID_HALF_SIZE);
        mouse_click_position_3D.y = static_cast<float>(random_y_in_grid - GRID_HALF_SIZE);
        mouse_click_position_3D.z = static_cast<float>(random_z_in_grid - GRID_HALF_SIZE);

        RayHitInfo ray_hit_info = GetCollisionRayGround(mouse_ray, ground_height);

        mouse_click_position_3D = ray_hit_info.position;
        std::printf("Creating object at %f, %f, %f\n", mouse_click_position_3D.x, mouse_click_position_3D.y, mouse_click_position_3D.z);

        constexpr uint8_t MAX_COLOR_COMPONENT = 255;
        Color color
        {
            .r = random_number_generator() % MAX_COLOR_COMPONENT,
            .g = random_number_generator() % MAX_COLOR_COMPONENT,
            .b = random_number_generator() % MAX_COLOR_COMPONENT,
            .a = MAX_COLOR_COMPONENT
        };

        Object3D cube =
        {
            .Type = ObjectType::CUBE,
            .SolidColor = color,
            .Cube =
            {
                .CenterPosition = mouse_click_position_3D,
                .Size = Vector3{ 1.0f, 1.0f, 1.0f }
            }
        };
        g_scene.Objects.emplace_back(cube);
    }

    // DRAW.
    BeginDrawing();
    ClearBackground(g_scene.BackgroundColor);

    BeginMode3D(g_camera);
    {
        DrawGrid(10, 1.0f);

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
            }
        }

        if (ray_to_view)
        {
            DrawRay(*ray_to_view, VIOLET);
        }
    }
    EndMode3D();

    DrawFPS(10, 10);

    EndDrawing();
}

int main()
{
    // GENERIC INIT.
    constexpr int SCREEN_WIDTH_IN_PIXELS = 600;
    constexpr int SCREEN_HEIGHT_IN_PIXELS = 400;
    InitWindow(SCREEN_WIDTH_IN_PIXELS, SCREEN_HEIGHT_IN_PIXELS, "raylib");
    SetConfigFlags(FLAG_WINDOW_RESIZABLE);

    // SPECIFIC INIT.
    g_camera.position = { 0.0f, 5.0f, 10.0f };
    g_camera.target = { 0.0f, 0.0f, 0.0f };
    g_camera.up = { 0.0f, 1.0f, 0.0f };
    g_camera.fovy = 45.0f;
    g_camera.type = CAMERA_PERSPECTIVE;

    SetCameraMode(g_camera, CAMERA_FREE);

#if DEFAULT_SCENE
    Object3D line =
    {
        .Type = ObjectType::LINE,
        .SolidColor = GOLD,
        .Line = 
        {
            .StartPosition = Vector3{ -5.0f, -1.0f, -1.0f },
            .EndPosition = Vector3{ -4.0f, 1.0f, 1.0f },
        }
    };
    g_scene.Objects.emplace_back(line);

    Object3D cube =
    {
        .Type = ObjectType::CUBE,
        .SolidColor = MAROON,
        .Cube = 
        {
            .CenterPosition = Vector3{ -3.0f, 1.0f, 0.0f },
            .Size = Vector3{ 1.0f, 1.0f, 1.0f }
        }
    };
    g_scene.Objects.emplace_back(cube);

    Object3D sphere =
    {
        .Type = ObjectType::SPHERE,
        .SolidColor = DARKGREEN,
        .Sphere = 
        {
            .CenterPosition = Vector3{ 0.0f, 0.0f, 0.0f },
            .Radius = 0.5f
        }
    };
    g_scene.Objects.emplace_back(sphere);

    Object3D cylinder =
    {
        .Type = ObjectType::CYLINDER,
        .SolidColor = DARKBLUE,
        .Cylinder = 
        {
            .CenterPosition = Vector3{ 2.0f, 1.0f, -1.0f },
            .TopRadius = 0.5f,
            .BottomRadius = 1.0f,
            .Height = 1.0f,
            .SliceCount = 16,
        }
    };
    g_scene.Objects.emplace_back(cylinder);
#endif

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