#include "Graphics.hpp"

Graphics::Graphics()
{
    // Mark this component as a graphics-type component
    // so GameObject knows to route draw() to it.
    m_IsGraphics = true;
}
