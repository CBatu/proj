#pragma once

class IApplication {
public:
    virtual ~IApplication() = default;

    virtual void OnInit() = 0;
    virtual void OnUpdate(float dt) = 0;
    virtual void OnRender() = 0;
    virtual void OnShutdown() = 0;
};
