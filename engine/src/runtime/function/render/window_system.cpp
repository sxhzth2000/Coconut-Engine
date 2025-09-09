//
// Created by tanhao on 2025/8/23.
//

#include "window_system.h"
#include "runtime/core/base/macro.h"

namespace Coconut
{
    WindowSystem::~WindowSystem() {};
    void WindowSystem::initialize(WindowCreateInfo create_info)
    {

        if (!glfwInit())
        {
            //     LOG_FATAL(__FUNCTION__, "failed to initialize GLFW");
            return;
        }

        m_width  = create_info.width;
        m_height = create_info.height;

        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        m_window = glfwCreateWindow(create_info.width, create_info.height, create_info.title, nullptr, nullptr);

        if (!m_window)
        {
            LOG_FATAL(__FUNCTION__, "failed to create window");
            glfwTerminate();
            return;
        }

        // Setup input callbacks
        glfwSetWindowUserPointer(m_window, this);
        glfwSetWindowCloseCallback(m_window, windowCloseCallback);

    }

    bool WindowSystem::shouldClose() const { return glfwWindowShouldClose(m_window); }
    GLFWwindow* WindowSystem::getWindow() const { return m_window; }

} // namespace Coconut