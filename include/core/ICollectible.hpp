#pragma once

#include <cstdint>

/**
 * @brief Interface describing a collectible item placed on the level grid.
 *
 * `LevelManager` stores only tile identifiers, while implementations of this
 * interface define the gameplay behavior of pellets, power pellets, and fruit.
 */
class ICollectible {
   public:
    /**
     * @brief Virtual destructor for polymorphic collectible implementations.
     */
    virtual ~ICollectible() = default;

    /**
     * @brief Returns the score granted when this collectible is picked up.
     *
     * @return Number of points awarded to the player.
     */
    virtual int points() const = 0;
    /**
     * @brief Returns the frightened-mode duration triggered by this collectible.
     *
     * A value greater than zero enables the player power timer and frightened
     * ghost behavior.
     *
     * @return Duration in seconds, or `0.f` if the collectible has no effect.
     */
    virtual float frightenedModeDuration() const {
        return 0.f;
    }
    /**
     * @brief Indicates whether this item counts toward level completion.
     *
     * @return `true` for pellets that must be cleared, `false` otherwise.
     */
    virtual bool countsTowardLevelPelletTotal() const {
        return true;
    }
    /**
     * @brief Returns the numeric tag sent over the network when the tile clears.
     *
     * @return Snapshot tile tag used by the client to mirror the clear event.
     */
    virtual std::uint8_t networkClearTag() const {
        return 1;
    }
};

/**
 * @brief Returns the collectible definition associated with a raw tile value.
 *
 * @param typeRaw Raw integer matching a `TileType` enum value.
 * @return Pointer to the collectible definition, or `nullptr` if the tile is
 * not collectible.
 */
const ICollectible* collectibleForTileType(int typeRaw);
