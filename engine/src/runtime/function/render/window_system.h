//
// Created by tanhao on 2025/8/23.
//

#ifndef COCONUT_WINDOW_SYSTEM_H
#define COCONUT_WINDOW_SYSTEM_H
#include "GLFW/glfw3.h"

namespace Coconut
{

    struct WindowCreateInfo
    {
        int         width{1280};
        int         height{720};
        const char* title{"Coconut"};
        bool        is_fullscreen{false};
    };

    class WindowSystem
    {
    public:
        WindowSystem() = default;
        ~WindowSystem();
        void initialize(WindowCreateInfo create_info);
        GLFWwindow*        getWindow() const;
        void log();

    public:


        bool               shouldClose() const;

    private:
        GLFWwindow* m_window{nullptr};
        int         m_width{0};
        int         m_height{0};

        bool m_is_focus_mode{false};

    protected:

        static void windowCloseCallback(GLFWwindow* window) { glfwSetWindowShouldClose(window, true); }


    };

} // namespace Coconut

#endif // COCONUT_WINDOW_SYSTEM_H
