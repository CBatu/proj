#pragma once
#include "gfx/renderer.h"

class Game {
public:

    void OnInit();
    void OnUpdate(float dt);
    void OnRender();
    void OnShutdown();
};
