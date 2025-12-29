#pragma once
#include <iostream>

// -------------------------------------------------------------
// Component (base class)
//  - All Graphics and Update classes inherit from this.
//  - Only stores two flags that identify derived behavior.
// -------------------------------------------------------------
class Component
{
  public:
    bool m_IsGraphics = false;
    bool m_IsUpdate = false;
};