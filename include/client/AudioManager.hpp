#pragma once

#include <SFML/Audio.hpp>
#include <array>
#include <cstddef>
#include <string>

#include "shared/AudioCue.hpp"

/**
 * @brief Loads and plays bundled client-side game audio.
 *
 * Short effects use a small round-robin `sf::Sound` pool, while looping audio
 * such as the frightened siren and gameplay theme use `sf::Music`.
 */
class AudioManager {
   public:
    /**
     * @brief Loads bundled audio assets from known repository-relative paths.
     *
     * @return `true` if at least the required audio assets were loaded well
     * enough for playback to function.
     */
    bool tryLoadBundled();

    /**
     * @brief Plays one short audio cue immediately, if available.
     *
     * @param cue Cue identifier to play.
     */
    void playCue(AudioCue cue);

    /**
     * @brief Starts or stops the frightened-mode looping siren.
     *
     * @param on `true` to enable the loop, `false` to stop it.
     */
    void setFrightenedLoop(bool on);

    /**
     * @brief Plays the level intro jingle once, if it is loaded.
     */
    void playLevelIntro();

    /**
     * @brief Stops all currently managed music playback.
     */
    void stopAllMusic();

    /**
     * @brief Enables or disables the looping round background music.
     *
     * The theme is paused while the frightened siren is active.
     *
     * @param active `true` to keep round music active, `false` to stop it.
     */
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
