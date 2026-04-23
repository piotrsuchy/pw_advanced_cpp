#pragma once

#include <cstdint>

// Describes a tile-granted pickup (pellet, power pellet, fruit). Grid storage remains
// TileType in LevelManager; this type owns *behavior* and scoring for each collectible.
class ICollectible {
   public:
    virtual ~ICollectible() = default;

    virtual int points() const = 0;
    // >0: Pac-Man and ghosts enter frightened / power-timer for this many seconds.
    virtual float frightenedModeDuration() const {
        return 0.f;
    }
    // Only normal + power pellets count for "all pellets eaten" / level clear.
    virtual bool countsTowardLevelPelletTotal() const {
        return true;
    }
    // SNAPSHOT / client clear path: 1 = normal pellet, 2 = power pellet; bonus fruit use 4–7.
    virtual std::uint8_t networkClearTag() const {
        return 1;
    }
};

// Registry for TileType pellet / power / bonus fruit (int matches enum class).
const ICollectible* collectibleForTileType(int typeRaw);
