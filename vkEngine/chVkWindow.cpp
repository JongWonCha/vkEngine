#include "chVkWindow.hpp"

#include <stdexcept>

namespace chVk
{
    ChVkWindow::ChVkWindow(int width, int height, std::string windowName): _width(width), _height(height),
                                                                         _windowName(windowName)
    {
        InitWindow();
    }

    ChVkWindow::~ChVkWindow()
    {
        glfwDestroyWindow(_window);
        glfwTerminate();
    }

    void ChVkWindow::InitWindow()
    {
        glfwInit();
        glfwWindowHint(GLFW_CLIENT_API, GLFW_NO_API);
        glfwWindowHint(GLFW_RESIZABLE, GLFW_TRUE);

        _window = glfwCreateWindow(_width, _height, _windowName.c_str(), nullptr, nullptr);
        glfwSetWindowUserPointer(_window, this);
        glfwSetFramebufferSizeCallback(_window, FrameBufferResizeCallback);
    }

    void ChVkWindow::FrameBufferResizeCallback(GLFWwindow* window, int width, int height)
    {
        auto currWindow = reinterpret_cast<ChVkWindow*>(glfwGetWindowUserPointer(window));
        currWindow->_frameBufferResized = true;
        currWindow->_width = width;
        currWindow->_height = height;
    }

    void ChVkWindow::CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface)
    {
        if (glfwCreateWindowSurface(instance, _window, nullptr, surface) != VK_SUCCESS )
        {
            throw std::runtime_error("Failed to create window surface!");
        }
    }
}
