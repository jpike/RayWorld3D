#include <cstdio>
#if __EMSCRIPTEN__
#include <emscripten.h>
#endif
#define RAYGUI_IMPLEMENTATION 1
#include <raylib/raygui.h>
#include <raylib/raylib.h>
#include <raylib/raymath.h>

Camera3D g_camera;

class Scene
{
public:
    Color BackgroundColor = BLACK;
};

Scene g_scene;

void UpdateAndDrawFrame()
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

void UpdateAndDrawFrameOld()
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

    Rectangle window_bounding_box =
    {
        .x = 10,
        .y = 20,
        .width = 200,
        .height = 100
    };
    GuiWindowBox(window_bounding_box, "GUI Window");

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