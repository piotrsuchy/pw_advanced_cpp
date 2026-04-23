#pragma once

#include <SFML/Audio.hpp>
#include <array>
#include <cstddef>
#include <string>

#include "shared/AudioCue.hpp"

// Loads bundled WAV/OGG from repo-relative roots (same pattern as HUD fonts).
// Short cues use a small round-robin sf::Sound pool; looping siren uses sf::Music.
class AudioManager {
   public:
    bool tryLoadBundled();

    void playCue(AudioCue cue);

    void setFrightenedLoop(bool on);

    void playLevelIntro();

    void stopAllMusic();

    /// Looping gameplay theme (MP3). No-op if file missing. Paused while frightened siren plays.
    void setRoundBgm(bool active);

   private:
    std::string resolveFirstExisting(const char* fileName) const;
    void        startThemeIfNeeded_();

    sf::SoundBuffer pelletChomp_;
    sf::SoundBuffer powerPellet_;
    sf::SoundBuffer ghostEaten_;
    sf::SoundBuffer death_;
    sf::SoundBuffer introJingle_;
    bool            hasPellet_{false};
    bool            hasPower_{false};
    bool            hasGhost_{false};
    bool            hasDeath_{false};
    bool            hasIntro_{false};

    std::array<sf::Sound, 8> voicePool_{};
    std::size_t              nextVoice_{0};

    sf::Music frightenedLoop_;
    bool      frightenedLoopLoaded_{false};
    bool      frightenedPlaying_{false};

    sf::Music themeLoop_;
    bool      themeLoaded_{false};
    bool      roundBgmActive_{false};
    bool      themePausedForFrightened_{false};
};
