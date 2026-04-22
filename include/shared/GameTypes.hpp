#pragma once

// Shared game enums and basic types that are free of graphics dependencies.

enum class Direction { None, Up, Down, Left, Right };

using TickId = unsigned int;

struct Vec2 {
    float x{0.f};
    float y{0.f};
};
