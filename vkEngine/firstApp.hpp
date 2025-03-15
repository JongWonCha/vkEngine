#pragma once

#include "chVkWindow.hpp"
#include "chVkDevice.hpp"
#include "chVkGameObject.hpp"
#include "chVkRenderer.hpp"

// std
#include <memory>
#include <vector>

namespace chVk
{
    class FirstApp
    {
    public:
        static constexpr  int WIDTH = 800;
        static constexpr  int HEIGHT = 600;

        FirstApp();
        ~FirstApp();

        FirstApp(const FirstApp&) = delete;
        FirstApp &operator=(const FirstApp&) = delete;

        void Run();
    private:
        void LoadGameObjects();
        
        ChVkWindow                      _chVkWindow{WIDTH, HEIGHT, "Hello Vulkan!"};
        ChVkDevice                      _chVkDevice{_chVkWindow};
		ChVkRenderer					_chVkRenderer{ _chVkWindow, _chVkDevice };

        std::vector<chVkGameObject>     _chVkGameObjects;
    };
}