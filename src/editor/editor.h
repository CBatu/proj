#pragma once


#include "core/application.h"

class Editor : public IApplication {
public:
    void OnInit() override;
    void OnUpdate(float dt) override;
    void OnRender() override;
    void OnShutdown() override;
};
