#pragma once
#ifndef TEXTURE_HOLDER_H
#define TEXTURE_HOLDER_H
#include <SFML/Graphics.hpp>
#include <map>
#include <string>

class TextureHolder
{
  public:
    TextureHolder();

    static sf::Texture &GetTexture(const std::string &filename);

  private:
    // Map pairing filenames with Texture objects
    std::map<std::string, sf::Texture> m_Textures;

    // Single shared instance
    static TextureHolder *m_s_Instance;
};
#endif