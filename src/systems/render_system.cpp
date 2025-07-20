#include "render_system.h"

#include "position.h"
#include "renderable.h"
#include <raylib.h>

namespace bk {
    void register_render_system(flecs::world &world) {
        auto r = world.system<Position, Renderable>().each(
                [](flecs::iter &it, size_t, Position &pos, Renderable &tex) {
                DrawTexture(tex.texture, pos.x, pos.y, WHITE);
                });
    }
}
