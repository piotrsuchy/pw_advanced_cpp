#pragma once

// Shared identifiers for gameplay audio. Playback is client-only (SFML); the server
// never links audio — cues are derived from snapshot deltas on the client.
enum class AudioCue {
    PelletChomp,
    PowerPellet,
    GhostEaten,
    PacmanDeath,
    LevelIntroJingle,
};
