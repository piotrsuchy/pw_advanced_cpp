#include "core/ICollectible.hpp"

#include "core/LevelManager.hpp"

namespace {

    class Pellet : public ICollectible {
       public:
        int points() const override {
            return 10;
        }
        bool countsTowardLevelPelletTotal() const override {
            return true;
        }
    };

    class PowerPellet : public ICollectible {
       public:
        int points() const override {
            return 50;
        }
        float frightenedModeDuration() const override {
            return 10.f;
        }
        std::uint8_t networkClearTag() const override {
            return 2;
        }
        bool countsTowardLevelPelletTotal() const override {
            return true;
        }
    };

    class Cherry : public ICollectible {
       public:
        int points() const override {
            return 100;
        }
        bool countsTowardLevelPelletTotal() const override {
            return false;
        }
    };

    static const Pellet      kPellet;
    static const PowerPellet kPowerPellet;
    static const Cherry      kCherry;

}  // namespace

const ICollectible* collectibleForTileType(int typeRaw) {
    switch (static_cast<TileType>(typeRaw)) {
        case TileType::Pellet:
            return &kPellet;
        case TileType::PowerPellet:
            return &kPowerPellet;
        case TileType::Cherry:
            return &kCherry;
        default:
            return nullptr;
    }
}
