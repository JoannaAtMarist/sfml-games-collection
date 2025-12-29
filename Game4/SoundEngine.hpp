#pragma once
#include <memory>
#include <map>
#include <string>
#include <SFML/Audio.hpp>

class SoundEngine
{

  private:
    // The singleton instance pointer
    static SoundEngine *m_s_Instance;

    // Buffers hold the audio data in memory
    std::map<std::string, sf::SoundBuffer> m_Buffers;

    // The background music streamer
    sf::Music m_Music;

  public:
    SoundEngine();

    // Singleton accessor
    static SoundEngine *getInstance();

    // Sound Effects
    static void playClick();
    static void playJump();
    static void playDeath();

    // Music Controls
    static void startMusic();
    static void pauseMusic();
    static void stopMusic();

    // Add a function to switch music
    static void playLevelMusic(int level);
};