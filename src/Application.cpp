#include <GL/glew.h>
#include <GLFW/glfw3.h>

#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

#include "Renderer.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "IndexBuffer.h"
#include "VertexArray.h"
#include "Shader.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"

#include "imgui/imgui.h"
#include "imgui/imgui_impl_glfw_gl3.h"

#include "tests/Test—oursework.h"


void framebuffer_resize_callback(GLFWwindow* window, int width, int height)
{	
	glViewport(0, 0, width, height);
	std::cout << "New frame size: " << width << "x" << height << std::endl;
}

int main(void)
{
	GLFWwindow* window;

	if (!glfwInit())
		return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	//glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);	// CORE == ÔÓ ÛÏÓÎ˜‡ÌË˛ VAO ÌÂ ·Û‰ÂÚ ÒÓÁ‰‡‚‡Ú¸Òˇ
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_COMPAT_PROFILE);	// debug mode

	window = glfwCreateWindow(960, 540, "Hello World", NULL, NULL);
	if (!window)
	{
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSwapInterval(1);	

	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

	if (glewInit() != GLEW_OK)	
		std::cout << "Error in glewInit()!" << std::endl;

	{	

		ImGui::CreateContext();
		ImGui_ImplGlfwGL3_Init(window, true);
		ImGui::StyleColorsDark();

		test::Test—oursework test(window);

		while (!glfwWindowShouldClose(window))
		{

			ImGui_ImplGlfwGL3_NewFrame();

			// Our test
			{	
				test.OnUpdate(1.f/40);
				test.OnRenderer();
				test.OnImGuiRenderer();
			}
			
			ImGui::Render();
			ImGui_ImplGlfwGL3_RenderDrawData(ImGui::GetDrawData());

			glfwSwapBuffers(window);
			glfwPollEvents();
		}
	}

	ImGui_ImplGlfwGL3_Shutdown();
	ImGui::DestroyContext();
	glfwTerminate();
	return 0;
}