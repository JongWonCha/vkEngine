#include "lvewindow.hpp"

#include <stdexcept>

namespace lve
{
    LveWindow::LveWindow(int width, int height, std::string windowName): _width(width), _height(height),
                                                                         _windowName(windowName)
    {
        InitWindow();
    }

    LveWindow::~LveWindow()
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void LveWindow::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

        _window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
    }

    void LveWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS )
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    }
}
