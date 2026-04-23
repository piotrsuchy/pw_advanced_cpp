#pragma once

class Simulation;

/**
 * @brief High-level match states used by the server and client flow.
 */
enum class MatchPhase {
    Waiting,        ///< Waiting for clients or ready-up before play starts.
    Playing,        ///< Active gameplay is in progress.
    LevelComplete,  ///< The round ended because the level was cleared.
    GameOver,       ///< The round ended because all remaining lives were lost.
};

/**
 * @brief Tracks the coarse-grained state of a multiplayer round.
 *
 * `Match` does not own world simulation; it only derives the current phase
 * from authoritative `Simulation` outcomes and lobby transitions.
 */
class Match {
   public:
    /**
     * @brief Creates a match in the requested initial phase.
     *
     * @param initial Starting match phase.
     */
    explicit Match(MatchPhase initial = MatchPhase::Playing);

    /**
     * @brief Updates the match phase after a simulation step.
     *
     * @param sim Authoritative simulation to inspect for win/lose conditions.
     */
    void updateAfterStep(const Simulation& sim);

    /**
     * @brief Returns the current match phase.
     *
     * @return Current phase.
     */
    MatchPhase phase() const {
        return phase_;
    }

    /**
     * @brief Checks whether the match has ended in game over.
     *
     * @return `true` if the phase is `MatchPhase::GameOver`.
     */
    bool isGameOver() const {
        return phase_ == MatchPhase::GameOver;
    }

    /**
     * @brief Checks whether the match has ended in level completion.
     *
     * @return `true` if the phase is `MatchPhase::LevelComplete`.
     */
    bool isLevelComplete() const {
        return phase_ == MatchPhase::LevelComplete;
    }

    /**
     * @brief Forces the match into the active playing state.
     */
    void resetToPlaying() {
        phase_ = MatchPhase::Playing;
    }

    /**
     * @brief Forces the match back into the waiting state.
     */
    void resetToWaiting() {
        phase_ = MatchPhase::Waiting;
    }

    /**
     * @brief Starts a round if the match is still in the waiting phase.
     */
    void beginFromWaiting() {
        if (phase_ == MatchPhase::Waiting) phase_ = MatchPhase::Playing;
    }

   private:
    MatchPhase phase_;
};
