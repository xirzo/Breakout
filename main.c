#include <raylib.h>
#include <logger.h>
#include "flecs.h"

#define SCREEN_WIDTH  800
#define SCREEN_HEIGHT 600

#define PLATFORM_MARGIN 100.f

typedef struct {
    Texture2D texture;
} bkTexture;

typedef struct {
    Vector2 position;
} bkPosition;

static void DrawTextureSystem(ecs_iter_t *it);

int main(void) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "breakout");
    SetTargetFPS(60);

    ecs_world_t *world = ecs_init();

    Texture2D platform_texture = LoadTexture("platform.png");

    ECS_COMPONENT(world, bkPosition);
    ECS_COMPONENT(world, bkTexture);

    ECS_SYSTEM(
        world, DrawTextureSystem, EcsOnUpdate, [in] bkPosition, [in] bkTexture
    );

    ecs_entity_t player = ecs_new(world);

    ecs_set(
        world,
        player,
        bkPosition,
        { .position = { (SCREEN_WIDTH - platform_texture.width) / 2.f,
                        (SCREEN_HEIGHT - platform_texture.height) / 2.f
                            + PLATFORM_MARGIN }

        }
    );

    ecs_set(
        world,
        player,
        bkTexture,
        {
            .texture = platform_texture,
        }
    );

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        ecs_progress(world, GetFrameTime());
        // ecs_run(world, ecs_id(DrawTextureSystem), GetFrameTime(), NULL);

        EndDrawing();
    }

    UnloadTexture(platform_texture);
    CloseWindow();
    ecs_fini(world);
    return 0;
}

void DrawTextureSystem(ecs_iter_t *it) {
    bkPosition *positions = ecs_field(it, bkPosition, 0);
    bkTexture  *textures = ecs_field(it, bkTexture, 1);

    for (int i = 0; i < it->count; i++) {
        Vector2   *pos = &positions[i].position;
        Texture2D *tex = &textures[i].texture;

        DrawTextureV(*tex, *pos, RED);
    }
}
