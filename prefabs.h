#ifndef PREFABS_H
#define PREFABS_H

#include "flecs.h"

// TODO: create player prefab

ecs_entity_t construct_player_prefab(ecs_entity_t *world) {
    ecs_entity_t player_prefab = ecs_entity(
        *world,
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

    return player_prefab;
}

#endif  // !PREFABS_H
