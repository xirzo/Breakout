#include <raylib.h>
#include <logger.h>
#include "flecs.h"

#include "input.h"
#include "renderable.h"
#include "velocity.h"
#include "position.h"

#include "movement_system.h"
#include "render_system.h"
#include "input_system.h"

#define DEBUGGING

constexpr int SCREEN_WIDTH  = 1920;
constexpr int SCREEN_HEIGHT = 1200;
constexpr int PLATFORM_MARGIN = 100.f;

int main(void) {
    SetConfigFlags(FLAG_FULLSCREEN_MODE);  
    SetTraceLogLevel(LOG_NONE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "breakout");
    SetTargetFPS(60);

    flecs::world world;

    Texture2D platform_texture = LoadTexture("platform.png");

    auto player_prefab =
        world.prefab()
            .set<bk::Position>( { (SCREEN_WIDTH - platform_texture.width) / 2.f, (SCREEN_HEIGHT - platform_texture.height) / 2.f + PLATFORM_MARGIN})
            .set<bk::Velocity>({ 0, 0, 400.f })
            .set<bk::Input>({ KEY_J, KEY_K })
            .set<bk::Renderable>({ platform_texture });

    auto player_1 = world.entity().is_a(player_prefab);

    bk::register_movement_system(world);
    bk::register_render_system(world);
    bk::register_input_system(world);

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BEIGE);

        world.progress();

        EndDrawing();
    }

    UnloadTexture(platform_texture);
    CloseWindow();
    return 0;
}
