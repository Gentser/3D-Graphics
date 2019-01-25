#include "TestLightning.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/gtc/matrix_transform.hpp"


namespace test {

	TestLightning::TestLightning()
		: m_Proj(glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 100.0f)),
		m_CamPos(glm::vec3(0.0f, 0.0f, -1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), m_CamPos)),
		m_TranslationA(1.0f, 0.0f, 0.0f), m_TranslationB(1.0f, 1.0f, 0.0f),
		m_lightPos0(glm::vec3(0.f, 0.f, -0.5f))
	{
		float positions[]{
			// Positions			// Text.coords	//Normals 
			-0.5f, -0.5f, 0.0f,		0.0f, 0.0f,		-0.0f, -0.0f, -0.5f,	// 0	
			 0.5f, -0.5f, 0.0f,		1.0f, 0.0f,		-0.0f, -0.0f, -0.5f,	// 1
			 0.5f,  0.5f, 0.0f,		1.0f, 1.0f,		-0.0f, -0.0f, -0.5f,	// 2
			-0.5f,  0.5f, 0.0f,		0.0f, 1.0f,		-0.0f, -0.0f, -0.5f,	// 3
		};

		unsigned int indeces[]{
			0, 1, 2,
			2, 3, 0
		};

		GLCall(glEnable(GL_BLEND));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

		m_VAO = std::make_unique<VertexArray>();
		m_VertexBuffer = std::make_unique<VertexBuffer>(positions, sizeof(positions));

		VertexBufferLayout layout;
		layout.Push<float>(3);	// Positions
		layout.Push<float>(2);	// Text. coords
		layout.Push<float>(3);	// Normals

		m_VAO->AddBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(indeces, 6);

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/explode.png");
		m_Shader->SetUniform1i("u_Texture", 0);

	}

	TestLightning::~TestLightning()
	{
		// Не нужно ничего удалять, т.к. используем умные указатели
	}

	void TestLightning::OnUpdate(float deltaTime)
	{
	}

	void TestLightning::OnRenderer()
	{
		Renderer renderer;
		renderer.Clear();

		m_Shader->Bind();
		m_Shader->SetUniform3f("u_LightPos0", m_lightPos0[0], m_lightPos0[1], m_lightPos0[2]);

		m_Texture->Bind();

		{

			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_ModelMatrix", model);
			m_Shader->SetUniformMat4f("u_ViewMatrix", m_View);
			m_Shader->SetUniformMat4f("u_ProjMatrix", m_Proj);
			m_Shader->SetUniform3f("u_CamPose", m_CamPos[0], m_CamPos[1], m_CamPos[2]);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_ModelMatrix", model);
			m_Shader->SetUniformMat4f("u_ViewMatrix", m_View);
			m_Shader->SetUniformMat4f("u_ProjMatrix", m_Proj);
			m_Shader->SetUniform3f("u_CamPose", m_CamPos[0], m_CamPos[1], m_CamPos[2]);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}

	void TestLightning::OnImGuiRenderer()
	{
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -2.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, -2.0f, 2.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}