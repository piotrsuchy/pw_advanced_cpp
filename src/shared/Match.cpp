#include "shared/Match.hpp"

#include "shared/Simulation.hpp"

Match::Match(MatchPhase initial) : phase_(initial) {}

void Match::updateAfterStep(const Simulation& sim) {
    if (phase_ == MatchPhase::GameOver || phase_ == MatchPhase::LevelComplete) return;
    if (phase_ == MatchPhase::Waiting) return;

    if (sim.isMatchLost()) {
        phase_ = MatchPhase::GameOver;
        return;
    }
    if (sim.isLevelCleared()) phase_ = MatchPhase::LevelComplete;
}
