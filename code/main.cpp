#include <cstdio>
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <raylib/raylib.h>
#include <raylib/raymath.h>

Camera3D g_camera;

void UpdateAndDrawFrame()
{
#if !__EMSCRIPTEN__
    // https://en.wikipedia.org/wiki/Table_of_keyboard_shortcuts
    if (IsKeyPressed(KEY_F11))
    {
        ToggleFullscreen();
    }
#endif

    Vector3 cube_position = { 0.0f, 0.0f, 0.0f };
    constexpr float CUBE_SIDE_LENGTH = 1.0f;
    constexpr float CUBE_HALF_SIDE_LENGTH = CUBE_SIDE_LENGTH / 2.0f;
    BoundingBox cube_bounding_box;
    cube_bounding_box.min = Vector3SubtractValue(cube_position, CUBE_HALF_SIDE_LENGTH);
    cube_bounding_box.max = Vector3AddValue(cube_position, CUBE_HALF_SIDE_LENGTH);
    bool collision = false;
#if MOUSE_PRESS_FOR_SELECT
    if (IsMouseButtonPressed(MOUSE_LEFT_BUTTON))
#endif
    {
        Vector2 mouse_position = GetMousePosition();
        /// @todo   Do after camera update?
        Ray mouse_ray = GetMouseRay(mouse_position, g_camera);
        collision = CheckCollisionRayBox(mouse_ray, cube_bounding_box);
        //std::printf("Collision!");
    }

    // UPDATE.
    UpdateCamera(&g_camera);

    // DRAW.
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(g_camera);
    {
        Color cube_color = RED;
        if (collision)
        {
            cube_color = BLUE;
        }
        DrawCube(cube_position, CUBE_SIDE_LENGTH, CUBE_SIDE_LENGTH, CUBE_SIDE_LENGTH, cube_color);
        DrawCubeWires(cube_position, CUBE_SIDE_LENGTH, CUBE_SIDE_LENGTH, CUBE_SIDE_LENGTH, PINK);
        DrawGrid(10, 1.0f);
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