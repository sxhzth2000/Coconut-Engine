//
// Created by tanhao on 2025/8/23.
//

#include "engine.h"

#include <iostream>

#include "runtime/core/base/macro.h"
#include "runtime/function/global/global_context.h"

void Coconut::CoconutEngine::startEngine()
{
    g_runtime_global_context.startSystems();
}

void Coconut::CoconutEngine::run()
{
    std::shared_ptr<WindowSystem> window_system= g_runtime_global_context.m_window_system;
    std::shared_ptr<RenderSystem> render_system= g_runtime_global_context.m_render_system;

    while (!window_system->shouldClose())
    {
        LOG_INFO("Begin tick");
        glfwPollEvents();
        render_system->tick(1);
        const float delta_time=1;
        LOG_INFO("End tick");
    }



}