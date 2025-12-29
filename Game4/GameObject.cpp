#include <iostream>
#include <SFML/Graphics.hpp>
#include "GameObject.hpp"
#include "Update.hpp"
#include "Graphics.hpp"

using namespace std;
using namespace sf;

// GameObject Implementation
void GameObject::addComponent(shared_ptr<Component> newComponent)
{
    m_Components.push_back(newComponent);
}

// Route update calls to Update components
void GameObject::update(float elapsedTime)
{
    for (auto component : m_Components) {
        if (component->m_IsUpdate) {
            static_pointer_cast<Update>(component)->update(elapsedTime);
        }
    }
}

// Route draw calls to Graphics components
void GameObject::draw(RenderWindow &window)
{
    for (auto component : m_Components) {
        if (component->m_IsGraphics) {
            static_pointer_cast<Graphics>(component)->draw(window);
        }
    }
}