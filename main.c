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
    Vector2 position;
} bkPosition;

typedef struct {
    Vector2 velocity;
    float   speed;
} bkVelocity;

typedef struct {
    KeyboardKey left;
    KeyboardKey right;
} bkInput;

static void DrawTextureSystem(ecs_iter_t *it);
static void InputSystem(ecs_iter_t *it);
static void MoveSystem(ecs_iter_t *it);

int main(void) {
    SetTraceLogLevel(LOG_NONE);
    InitWindow(SCREEN_WIDTH, SCREEN_HEIGHT, "breakout");
    SetTargetFPS(60);

    ecs_world_t *world = ecs_init();

    Texture2D platform_texture = LoadTexture("platform.png");

    ECS_COMPONENT(world, bkPosition);
    ECS_COMPONENT(world, bkVelocity);
    ECS_COMPONENT(world, bkTexture);
    ECS_COMPONENT(world, bkInput);

    ECS_SYSTEM(
        world, DrawTextureSystem, EcsOnUpdate, [in] bkPosition, [in] bkTexture
    );
    ECS_SYSTEM(world, InputSystem, EcsOnUpdate, [out] bkVelocity, [in] bkInput);

    ECS_SYSTEM(
        world, MoveSystem, EcsOnUpdate, [in] bkVelocity, [out] bkPosition
    );

    ecs_entity_t player_prefab = ecs_entity(
        world,
        {
            .add = ecs_ids(EcsPrefab),
        }
    );

    ecs_set(
        world,
        player_prefab,
        bkPosition,
        { .position = { (SCREEN_WIDTH - platform_texture.width) / 2.f,
                        (SCREEN_HEIGHT - platform_texture.height) / 2.f
                            + PLATFORM_MARGIN }

        }

    );

    ecs_set(
        world, player_prefab, bkVelocity, { .velocity = { 0, 0 }, .speed = 400 }
    );
    ecs_set(
        world,
        player_prefab,
        bkTexture,
        {
            .texture = platform_texture,
        }
    );

    ecs_set(
        world,
        player_prefab,
        bkInput,
        {
            .left = KEY_J,
            .right = KEY_K,
        }
    );

    ecs_entity_t player_inst_1 = ecs_new_w_pair(world, EcsIsA, player_prefab);
    ecs_entity_t player_inst_2 = ecs_new_w_pair(world, EcsIsA, player_prefab);

    ecs_set(world, player_inst_2, bkInput, { .left = KEY_A, .right = KEY_D });

    while (!WindowShouldClose()) {
        BeginDrawing();
        ClearBackground(BLACK);

        ecs_progress(world, GetFrameTime());

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

void InputSystem(ecs_iter_t *it) {
    bkVelocity *velocities = ecs_field(it, bkVelocity, 0);
    bkInput    *inputs = ecs_field(it, bkInput, 1);

    for (int i = 0; i < it->count; i++) {
        Vector2 *vel = &velocities[i].velocity;
        bkInput *inp = &inputs[i];

        *vel = (Vector2){ 0.f, 0.f };

        if (IsKeyDown(inp->left)) {
            *vel = (Vector2){ -1.f, 0.f };
        }
        if (IsKeyDown(inp->right)) {
            *vel = (Vector2){ 1.f, 0.f };
        }
    }
}

void MoveSystem(ecs_iter_t *it) {
    bkVelocity *velocities = ecs_field(it, bkVelocity, 0);
    bkPosition *positions = ecs_field(it, bkPosition, 1);

    for (int i = 0; i < it->count; i++) {
        Vector2 *vel = &velocities[i].velocity;
        float    speed = velocities[i].speed;
        Vector2 *pos = &positions[i].position;

        pos->x += vel->x * speed * it->delta_time;
        pos->y += vel->y * speed * it->delta_time;
    }
}

// TODO: create player prefab
