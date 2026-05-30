#include "AudioManager.h"

AudioManager::AudioManager()
    : m_currentTrack(MusicTrack::None)
{
}

void AudioManager::playMenuMusic()
{
    playMusicFile("assets/audio/Main_Menu_Theme.mp3", MusicTrack::Menu, 45.f);
}

void AudioManager::playDungeonMusic()
{
    playMusicFile("assets/audio/Gameplay_Theme.mp3", MusicTrack::Dungeon, 35.f);
}

void AudioManager::stopMusic()
{
    m_music.stop();
    m_currentTrack = MusicTrack::None;
}

void AudioManager::pauseMusic()
{
    if (m_currentTrack != MusicTrack::None)
    {
        m_music.pause();
    }
}

void AudioManager::resumeMusic()
{
    if (m_currentTrack != MusicTrack::None)
    {
        m_music.play();
    }
}

bool AudioManager::playMusicFile(const std::string& filePath, MusicTrack track, float volume)
{
    if (m_currentTrack == track)
    {
        return true;
    }

    m_music.stop();

    if (!m_music.openFromFile(filePath))
    {
        m_currentTrack = MusicTrack::None;
        return false;
    }

    m_music.setLooping(true);
    m_music.setVolume(volume);
    m_music.play();

    m_currentTrack = track;
    return true;
}