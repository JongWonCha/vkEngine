#include "firstApp.hpp"

namespace lve
{
    void FirstApp::Run()
    {
        while ( !_lveWindow.ShouldClose() )
        {
            glfwPollEvents();
        }
    }

}