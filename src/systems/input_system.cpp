#include "input_system.h"

#include "velocity.h"
#include "input.h"
#include <raylib.h>

namespace bk {
    void register_input_system(flecs::world &world) {
        auto input_sys = world.system<Velocity, Input>().each(
                [](flecs::iter &it, size_t, Velocity &vel, Input &inp) {
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
    }
}
