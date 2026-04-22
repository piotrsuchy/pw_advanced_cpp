#include "shared/InteractionResolver.hpp"

#include <algorithm>
#include <cmath>

bool InteractionResolver::circleCollide(Vec2 a, Vec2 b, float radius) {
    float dx = a.x - b.x;
    float dy = a.y - b.y;
    return (dx * dx + dy * dy) <= (radius * radius);
}

bool InteractionResolver::resolveLethal(std::array<Player, 2>& players, std::array<std::unique_ptr<Ghost>, 4>& ghosts,
                                        float collisionRadius) {
    bool anyKilled = false;

    for (int pi = 0; pi < 2; ++pi) {
        Player& player = players[pi];
        if (player.isDying()) continue;

        Vec2 ppos = player.getPacman().getPosition();

        for (int gi = 0; gi < 4; ++gi) {
            Ghost& ghost = *ghosts[gi];
            if (ghost.isFrightened()) continue;

            if (circleCollide(ppos, ghost.getPosition(), collisionRadius)) {
                player.loseLife();
                player.setDeathTimer(3.0f);
                anyKilled = true;
                break;
            }
        }
    }
    return anyKilled;
}

void InteractionResolver::resolveFrightened(std::array<Player, 2>&                 players,
                                            std::array<std::unique_ptr<Ghost>, 4>& ghosts, float collisionRadius,
                                            float ghostHomeX, float ghostHomeY, std::vector<GhostEatenEvent>& outEvents,
                                            std::array<float, 4>& ghostRespawnTimers) {
    bool anyPowered = players[0].isPowered() || players[1].isPowered();
    if (!anyPowered) return;

    for (int gi = 0; gi < 4; ++gi) {
        Ghost& ghost = *ghosts[gi];
        if (!ghost.isFrightened()) continue;

        for (int pi = 0; pi < 2; ++pi) {
            Player& player = players[pi];
            if (!player.isPowered()) continue;
            if (player.isDying()) continue;

            Vec2 ppos = player.getPacman().getPosition();
            if (circleCollide(ppos, ghost.getPosition(), collisionRadius)) {
                int count = player.getFrightenedEatCount();
                int pts   = GHOST_EAT_VALUES[std::min(count, 4)];
                player.addScore(pts);
                player.incrementFrightenedEatCount();

                ghost.setPosition(ghostHomeX, ghostHomeY);
                ghost.setFrightened(0.f);
                ghostRespawnTimers[gi] = 5.0f;

                outEvents.push_back({ghostHomeX, ghostHomeY, pts, gi});
                break;
            }
        }
    }
}
