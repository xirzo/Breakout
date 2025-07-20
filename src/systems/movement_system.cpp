#include "movement_system.h"

namespace bk {
    void register_movement_system(flecs::world &world) {
        auto move_system = world.system<bk::Position, bk::Velocity>().each(
            [](flecs::iter &it, size_t, bk::Position &pos, bk::Velocity &vel) {
                pos.x += vel.x * vel.speed * it.delta_time();
                pos.y += vel.y * vel.speed * it.delta_time();
            }
        );
    }
}
