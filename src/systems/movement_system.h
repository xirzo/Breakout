#pragma once

#include "flecs.h"

#include "position.h"
#include "velocity.h"

namespace bk {
    // void movement_system(const flecs::iter& it, Position* p, const Velocity* v);
    void register_movement_system(flecs::world &world);
}
