#pragma once
#define GLFW_INCLUDE_VULKAN
#include <string>
#include <GLFW/glfw3.h>

namespace lve
{
	class LveWindow
	{
	private:
		const int	_width;
		const int	_height;

		std::string _windowName;
		GLFWwindow* _window;

		void InitWindow();
	public:
		LveWindow(int width, int height, std::string windowName);
		~LveWindow();

		
		LveWindow(const LveWindow& other) = delete;
		LveWindow& operator=(const LveWindow& other) = delete;

		bool ShouldClose() const { return glfwWindowShouldClose(_window); }
		VkExtent2D GetExtent() const {return {static_cast<uint32_t>(_width), static_cast<uint32_t>(_height)};}
		void CreateWindowSurface(VkInstance instance, VkSurfaceKHR* surface);
	};
}