//
// Created by tanhao on 2025/8/23.
//

#ifndef COCONUT_GLOBAL_CONTEXT_H
#define COCONUT_GLOBAL_CONTEXT_H

#include <memory>
#include <string>


namespace Coconut
{
    class LogSystem;
    class WindowSystem;
    class RenderSystem;



    class RuntimeGlobalContext
    {
    public:
        // create all global systems and initialize these systems
        void startSystems();
        // destroy all global systems
        void shutdownSystems();

    public:
        std::shared_ptr<LogSystem>         m_logger_system;
        std::shared_ptr<WindowSystem>      m_window_system;
        std::shared_ptr<RenderSystem>      m_render_system;

    };

    extern RuntimeGlobalContext g_runtime_global_context;
}
#endif // COCONUT_GLOBAL_CONTEXT_H
