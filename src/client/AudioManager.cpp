#include "client/AudioManager.hpp"

#include <filesystem>
#include <iostream>

namespace {

    constexpr const char* kAudioRoots[] = {
        "assets/audio/", "./assets/audio/", "../assets/audio/", "../../assets/audio/", "../../../assets/audio/",
    };

}  // namespace

std::string AudioManager::resolveFirstExisting(const char* fileName) const {
    for (const char* dir : kAudioRoots) {
        std::filesystem::path p = std::filesystem::path(dir) / fileName;
        if (std::filesystem::is_regular_file(p)) return p.string();
    }
    return {};
}

bool AudioManager::tryLoadBundled() {
    auto loadBuf = [this](sf::SoundBuffer& buf, const char* fname, bool* okFlag) {
        const std::string path = resolveFirstExisting(fname);
        if (path.empty() || !buf.loadFromFile(path)) {
            *okFlag = false;
            return;
        }
        *okFlag = true;
    };

    loadBuf(pelletChomp_, "pellet_chomp.wav", &hasPellet_);
    loadBuf(powerPellet_, "power_pellet.wav", &hasPower_);
    loadBuf(ghostEaten_, "ghost_eaten.wav", &hasGhost_);
    loadBuf(death_, "death.wav", &hasDeath_);
    loadBuf(introJingle_, "intro_jingle.wav", &hasIntro_);

    std::string sirenPath;
    for (const char* cand : {"frightened_siren.mp3", "frightened_siren.wav"}) {
        sirenPath = resolveFirstExisting(cand);
        if (!sirenPath.empty()) break;
    }
    frightenedLoopLoaded_ = !sirenPath.empty() && frightenedLoop_.openFromFile(sirenPath) &&
                            frightenedLoop_.getDuration().asSeconds() > 0.f;

    const std::string themePath = resolveFirstExisting("pacman-theme.mp3");
    themeLoaded_ =
        !themePath.empty() && themeLoop_.openFromFile(themePath) && themeLoop_.getDuration().asSeconds() > 0.f;

    const bool any =
        hasPellet_ || hasPower_ || hasGhost_ || hasDeath_ || hasIntro_ || frightenedLoopLoaded_ || themeLoaded_;

    if (!any) {
        std::cerr << "[CLIENT] No audio assets found. Run from repo root or build dir (expects assets/audio/*.wav).\n";
        return false;
    }

    if (!hasPellet_) std::cerr << "[CLIENT] Missing pellet_chomp.wav — chomp disabled.\n";
    if (!hasPower_) std::cerr << "[CLIENT] Missing power_pellet.wav — power SFX disabled.\n";
    if (!hasGhost_) std::cerr << "[CLIENT] Missing ghost_eaten.wav — ghost eat SFX disabled.\n";
    if (!hasDeath_) std::cerr << "[CLIENT] Missing death.wav — death SFX disabled.\n";
    if (!hasIntro_) std::cerr << "[CLIENT] Missing intro_jingle.wav — intro jingle disabled.\n";
    if (!frightenedLoopLoaded_)
        std::cerr << "[CLIENT] Missing frightened_siren.mp3 (or .wav) — frightened loop disabled.\n";
    if (!themeLoaded_) std::cerr << "[CLIENT] Missing assets/audio/pacman-theme.mp3 — gameplay theme disabled.\n";

    return true;
}

void AudioManager::playCue(AudioCue cue) {
    const sf::SoundBuffer* buf = nullptr;
    switch (cue) {
        case AudioCue::PelletChomp:
            if (hasPellet_) buf = &pelletChomp_;
            break;
        case AudioCue::PowerPellet:
            if (hasPower_) buf = &powerPellet_;
            break;
        case AudioCue::GhostEaten:
            if (hasGhost_) buf = &ghostEaten_;
            break;
        case AudioCue::PacmanDeath:
            if (hasDeath_) buf = &death_;
            break;
        case AudioCue::LevelIntroJingle:
            if (hasIntro_) buf = &introJingle_;
            break;
    }
    if (!buf || buf->getSampleCount() == 0) return;

    sf::Sound& voice = voicePool_[nextVoice_ % voicePool_.size()];
    nextVoice_++;
    voice.setBuffer(*buf);
    voice.play();
}

void AudioManager::setFrightenedLoop(bool on) {
    if (on) {
        if (!frightenedLoopLoaded_) return;
        if (themeLoaded_ && themeLoop_.getStatus() == sf::SoundSource::Playing) {
            themeLoop_.pause();
            themePausedForFrightened_ = true;
        } else {
            themePausedForFrightened_ = false;
        }
        if (!frightenedPlaying_) {
            frightenedLoop_.setLoop(true);
            frightenedLoop_.setVolume(10.f);
            frightenedLoop_.play();
            frightenedPlaying_ = true;
        }
    } else {
        if (frightenedPlaying_) {
            frightenedLoop_.stop();
            frightenedPlaying_ = false;
        }
        if (themePausedForFrightened_) {
            themeLoop_.play();
            themePausedForFrightened_ = false;
        } else if (roundBgmActive_ && themeLoaded_) {
            startThemeIfNeeded_();
        }
    }
}

void AudioManager::playLevelIntro() {
    playCue(AudioCue::LevelIntroJingle);
}

void AudioManager::stopAllMusic() {
    roundBgmActive_           = false;
    themePausedForFrightened_ = false;
    if (themeLoaded_) themeLoop_.stop();
    if (frightenedPlaying_) {
        frightenedLoop_.stop();
        frightenedPlaying_ = false;
    }
}

void AudioManager::setRoundBgm(bool active) {
    roundBgmActive_ = active;
    if (!themeLoaded_) return;
    if (!active) {
        themePausedForFrightened_ = false;
        themeLoop_.stop();
        return;
    }
    if (frightenedPlaying_) return;
    startThemeIfNeeded_();
}

void AudioManager::startThemeIfNeeded_() {
    if (!themeLoaded_ || frightenedPlaying_) return;
    if (themeLoop_.getStatus() == sf::SoundSource::Playing) return;
    themeLoop_.setLoop(true);
    themeLoop_.setVolume(28.f);
    themeLoop_.play();
}
