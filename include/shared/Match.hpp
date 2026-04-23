#pragma once

class Simulation;

/// High-level match flow: lobby / active play / round outcome.
/// Owns game-phase state; the Simulation remains authoritative for physics and per-tick state.
enum class MatchPhase { Waiting, Playing, LevelComplete, GameOver };

class Match {
   public:
    explicit Match(MatchPhase initial = MatchPhase::Playing);

    /// After an authoritative `Simulation::step`, updates phase from win/lose conditions.
    void updateAfterStep(const Simulation& sim);

    MatchPhase phase() const {
        return phase_;
    }
    bool isGameOver() const {
        return phase_ == MatchPhase::GameOver;
    }
    bool isLevelComplete() const {
        return phase_ == MatchPhase::LevelComplete;
    }
    /// Used when a new level loads after a clear (future) or to enter rounds from a lobby.
    void resetToPlaying() {
        phase_ = MatchPhase::Playing;
    }
    void beginFromWaiting() {
        if (phase_ == MatchPhase::Waiting) phase_ = MatchPhase::Playing;
    }

   private:
    MatchPhase phase_;
};
