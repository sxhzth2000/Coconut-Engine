//
// Created by tanhao on 2025/8/23.
//

#include "global_context.h"

#include "runtime/function/render/window_system.h"
#include "runtime/function/render/render_system.h"
#include "runtime/core/base/macro.h"
#include "runtime/core/log/log_system.h"




namespace Coconut
{
    RuntimeGlobalContext g_runtime_global_context;

    void RuntimeGlobalContext::startSystems() {
//
        m_logger_system = std::make_shared<LogSystem>();
        m_window_system = std::make_shared<WindowSystem>();

        WindowCreateInfo window_create_info;
        m_window_system->initialize(window_create_info);

        m_render_system = std::make_shared<RenderSystem>();

        RenderSystemInitInfo render_init_info;
        render_init_info.window_system = m_window_system;
        m_render_system->initialize(render_init_info);


    }

    void RuntimeGlobalContext::shutdownSystems() {


        m_window_system.reset();


    }

}