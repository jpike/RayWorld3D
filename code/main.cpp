#if __EMSCRIPTEN__
#include <emscripten.h>
#endif
#include <raylib/raylib.h>

Camera3D g_camera;

void UpdateAndDrawFrame()
{
    // UPDATE.
    UpdateCamera(&g_camera);

    // DRAW.
    BeginDrawing();
    ClearBackground(BLACK);

    BeginMode3D(g_camera);
    {
        Vector3 cube_position = { 0.0f, 0.0f, 0.0f };
        DrawCube(cube_position, 1.0f, 1.0f, 1.0f, RED);
        DrawCubeWires(cube_position, 1.0f, 1.0f, 1.0f, PINK);
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
        UpdateAndDrawFrame();
    }
#endif

    CloseWindow();

    return 0;
}