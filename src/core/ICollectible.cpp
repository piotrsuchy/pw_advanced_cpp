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

    class BonusFruit1 : public ICollectible {
       public:
        int points() const override {
            return 100;
        }
        bool countsTowardLevelPelletTotal() const override {
            return false;
        }
        std::uint8_t networkClearTag() const override {
            return 4;
        }
    };

    class BonusFruit2 : public ICollectible {
       public:
        int points() const override {
            return 200;
        }
        bool countsTowardLevelPelletTotal() const override {
            return false;
        }
        std::uint8_t networkClearTag() const override {
            return 5;
        }
    };

    class BonusFruit3 : public ICollectible {
       public:
        int points() const override {
            return 400;
        }
        bool countsTowardLevelPelletTotal() const override {
            return false;
        }
        std::uint8_t networkClearTag() const override {
            return 6;
        }
    };

    class BonusFruit4 : public ICollectible {
       public:
        int points() const override {
            return 800;
        }
        bool countsTowardLevelPelletTotal() const override {
            return false;
        }
        std::uint8_t networkClearTag() const override {
            return 7;
        }
    };

    static const Pellet      kPellet;
    static const PowerPellet kPowerPellet;
    static const BonusFruit1 kBonusFruit1;
    static const BonusFruit2 kBonusFruit2;
    static const BonusFruit3 kBonusFruit3;
    static const BonusFruit4 kBonusFruit4;

}  // namespace

const ICollectible* collectibleForTileType(int typeRaw) {
    switch (static_cast<TileType>(typeRaw)) {
        case TileType::Pellet:
            return &kPellet;
        case TileType::PowerPellet:
            return &kPowerPellet;
        case TileType::BonusFruit1:
            return &kBonusFruit1;
        case TileType::BonusFruit2:
            return &kBonusFruit2;
        case TileType::BonusFruit3:
            return &kBonusFruit3;
        case TileType::BonusFruit4:
            return &kBonusFruit4;
        default:
            return nullptr;
    }
}
