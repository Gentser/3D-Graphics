#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"
#include "DepthTexture.h"

#include "glm/glm.hpp"
#include <memory>

#include <GL/glew.h>
#include <GLFW/glfw3.h>

namespace test { 

	class Test3DLightMap_OpenGL : public Test
	{
	public:
		Test3DLightMap_OpenGL(GLFWwindow* window);
		~Test3DLightMap_OpenGL();

		void OnUpdate(float deltaTime) override;
		void OnRenderer() override;
		void OnImGuiRenderer() override;

	private:
		void computeMatricesFromInputs(float deltaTime);
		glm::mat4 getProjectionMatrix() const { return m_Proj; };
		glm::mat4 getViewMatrix() const { return m_View; };

		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;	// m_IBO
		std::unique_ptr<Shader> m_BasicShader;
		std::unique_ptr<Shader> m_DepthShader;
		std::unique_ptr<Texture> m_Texture;
		std::unique_ptr<DepthTexture> m_DepthTexture;
		GLuint m_FramebufferName;

		std::vector<float> m_vertices;
		std::vector<unsigned int> m_indData;

		glm::mat4 m_Proj, m_View;

		glm::vec3 m_lightPos0;

		GLFWwindow* m_window;
		glm::vec3 m_position;	// Prety same

		float m_horizontalAngle;	// horizontal angle : toward -Z
		float m_verticalAngle;		// vertical angle : 0, look at the horizon
		float m_FoV;				// Initial Field of View
		float m_speed;				// N units / second == 3.0
		float m_mouseSpeed;

		bool debugMode;
	};

}