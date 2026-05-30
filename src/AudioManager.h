#pragma once

#include <SFML/Audio.hpp>

#include <string>

enum class MusicTrack
{
    None,
    Menu,
    Dungeon
};

class AudioManager
{
public:
    AudioManager();

    void playMenuMusic();
    void playDungeonMusic();
    void stopMusic();
    void pauseMusic();
    void resumeMusic();

private:
    sf::Music m_music;
    MusicTrack m_currentTrack;

    bool playMusicFile(const std::string& filePath, MusicTrack track, float volume);
};