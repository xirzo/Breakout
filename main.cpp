#include <raylib.h>
#include <logger.h>
#include "flecs.h"

#define SCREEN_WIDTH  900
#define SCREEN_HEIGHT 700

#define PLATFORM_MARGIN 100.f

typedef struct {
    Texture2D texture;
} bkTexture;

typedef struct {
    float x;
    float y;
} bkPosition;

typedef struct {
    float x;
    float y;
    float speed;
} bkVelocity;

typedef struct {
    KeyboardKey left;
    KeyboardKey right;
} bkInput;

int main(void) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "breakout");
    SetTargetFPS(60);

    flecs::world world;

    Texture2D platform_texture = LoadTexture("platform.png");

    auto player_prefab =
        world.prefab()
            .set<bkPosition>({ (SCREEN_WIDTH - platform_texture.width) / 2.f,
                               (SCREEN_HEIGHT - platform_texture.height) / 2.f
                                   + PLATFORM_MARGIN })
            .set<bkVelocity>({ 0, 0, 400.f })
            .set<bkInput>({ KEY_J, KEY_K })
            .set<bkTexture>({ platform_texture });

    auto player_1 = world.entity().is_a(player_prefab);

    auto move_system = world.system<bkPosition, bkVelocity>().each(
        [](flecs::iter &it, size_t, bkPosition &pos, bkVelocity &vel) {
            pos.x += vel.x * vel.speed * it.delta_time();
            pos.y += vel.y * vel.speed * it.delta_time();
        }
    );

    auto draw_sys = world.system<bkPosition, bkTexture>().each(
        [](flecs::iter &it, size_t, bkPosition &pos, bkTexture &tex) {
            DrawTexture(tex.texture, pos.x, pos.y, RED);
        }
    );

    auto input_sys = world.system<bkVelocity, bkInput>().each(
        [](flecs::iter &it, size_t, bkVelocity &vel, bkInput &inp) {
            vel.x = 0;
            vel.y = 0;

            if (IsKeyDown(inp.left)) {
                vel.x = -1;
            }
            if (IsKeyDown(inp.right)) {
                vel.x = 1;
            }
        }
    );

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        world.progress();

        EndDrawing();
    }

    UnloadTexture(platform_texture);
    CloseWindow();
    return 0;
}

// TODO: create player prefab
