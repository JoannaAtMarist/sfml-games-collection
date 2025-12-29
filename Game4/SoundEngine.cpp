#include <assert.h>
#include <iostream>
#include "SoundEngine.hpp"

using namespace sf;

// Initialize the static singleton instance pointer
SoundEngine *SoundEngine::m_s_Instance = nullptr;

// Constructor
SoundEngine::SoundEngine()
{
    assert(m_s_Instance == nullptr);
    m_s_Instance = this;

    // Load Buffers
    if (!m_Buffers["click"].loadFromFile("sound/click.wav"))
        std::cout << "Could not load click.wav\n";

    if (!m_Buffers["jump"].loadFromFile("sound/jump.wav"))
        std::cout << "Could not load jump.wav\n";

    if (!m_Buffers["death"].loadFromFile("sound/death.wav"))
        std::cout << "Could not load death.wav\n";

    // Setup the listener
    sf::Listener::setDirection(1.f, 0.f, 0.f);
}

// Singleton accessor
SoundEngine *SoundEngine::getInstance()
{
    return m_s_Instance;
}

// Sound Effects
void SoundEngine::playClick()
{
    static sf::Sound sound;
    if (m_s_Instance->m_Buffers.count("click")) {
        sound.setBuffer(m_s_Instance->m_Buffers["click"]);
        sound.play();
    }
}
void SoundEngine::playJump()
{
    static sf::Sound sound;
    if (m_s_Instance->m_Buffers.count("jump")) {
        sound.setBuffer(m_s_Instance->m_Buffers["jump"]);
        sound.play();
    }
}
void SoundEngine::playDeath()
{
    static sf::Sound sound;
    if (m_s_Instance->m_Buffers.count("click")) {
        sound.setBuffer(m_s_Instance->m_Buffers["click"]);
        sound.setPitch(0.5f); // Low pitch click = death sound
        sound.play();
    }
}

// Music Controls
void SoundEngine::startMusic()
{
    if (m_s_Instance->m_Music.openFromFile("music/MattSideScrollPart1.ogg")) {
        m_s_Instance->m_Music.setLoop(true);
        m_s_Instance->m_Music.play();
    } else {
        std::cout << "Could not load music/MattSideScrollPart1.ogg\n";
    }
}
void SoundEngine::pauseMusic()
{
    m_s_Instance->m_Music.pause();
}
void SoundEngine::stopMusic()
{
    m_s_Instance->m_Music.stop();
}

// Switch music based on level
void SoundEngine::playLevelMusic(int level)
{
    // Stop old music
    m_s_Instance->m_Music.stop();

    std::string filename;
    if (level == 1) {
        filename = "music/MattSideScrollPart1.ogg";
    } else if (level == 2) {
        filename = "music/MattSideScrollPart2.ogg";
    }

    if (m_s_Instance->m_Music.openFromFile(filename)) {
        m_s_Instance->m_Music.setLoop(true);
        m_s_Instance->m_Music.play();
    } else {
        std::cout << "Could not load music for level " << level << "\n";
    }
}