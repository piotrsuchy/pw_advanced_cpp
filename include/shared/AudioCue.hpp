#pragma once

/**
 * @brief Identifiers for gameplay audio events recognized by the client.
 *
 * Playback is client-only; the server never links audio and instead exposes
 * enough state for the client to infer which cue should be played.
 */
enum class AudioCue {
    PelletChomp,       ///< Standard pellet collection sound.
    PowerPellet,       ///< Power pellet pickup sound.
    GhostEaten,        ///< Frightened ghost score sound.
    PacmanDeath,       ///< Pac-Man death sound.
    LevelIntroJingle,  ///< Intro jingle played before the round begins.
};
