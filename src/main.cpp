#include "core/engine.h"
#include "editor/editor.h"
#include "game/game.h"

int main(int argc, char** argv) {
    IApplication* app = nullptr;

    if (argc > 1 && std::string(argv[1]) == "--editor")
        app = new Editor();
    else
        app = new Game();
    Engine engine(1280, 720, "My Game Engine",app);
    engine.Run();
    return 0;
}
