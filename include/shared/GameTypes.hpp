#pragma once

/**
 * @brief Cardinal movement directions used across gameplay and rendering.
 */
enum class Direction {
    None,   ///< No movement or facing direction.
    Up,     ///< Upward movement.
    Down,   ///< Downward movement.
    Left,   ///< Leftward movement.
    Right,  ///< Rightward movement.
};

/**
 * @brief Server-side ghost mode outside of frightened behavior.
 */
enum class GhostMode {
    Scatter,  ///< Ghost targets its fixed corner tile.
    Chase,    ///< Ghost targets a player-dependent chase tile.
};

/**
 * @brief Monotonic identifier type for networked or simulated ticks.
 */
using TickId = unsigned int;

/**
 * @brief Lightweight 2D position used by logic code without graphics types.
 */
struct Vec2 {
    float x{0.f};  ///< Horizontal coordinate.
    float y{0.f};  ///< Vertical coordinate.
};
