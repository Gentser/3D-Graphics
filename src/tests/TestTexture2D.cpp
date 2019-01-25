#include "TestTexture2D.h"

#include "Renderer.h"
#include "imgui/imgui.h"

#include "glm/gtc/matrix_transform.hpp"


namespace test {

	TestTexture2D::TestTexture2D()
		: m_Proj(glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 1.0f)),
		m_View(glm::translate(glm::mat4(1.0f), glm::vec3(0.0f, 0.0f, 0.0f))),
		m_TranslationA(1.0f, 0.0f, 0.0f), m_TranslationB(1.0f, 1.0f, 0.0f)
	{
		float positions[]{
			-0.5f, -0.5f, 0.0f, 0.0f,	// 0	
			 0.5f, -0.5f, 1.0f, 0.0f,	// 1
			 0.5f,  0.5f, 1.0f, 1.0f,	// 2
			-0.5f,  0.5f, 0.0f, 1.0f,	// 3
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
		layout.Push<float>(2);	
		layout.Push<float>(2);

		m_VAO->AddBuffer(*m_VertexBuffer, layout);
		m_IndexBuffer = std::make_unique<IndexBuffer>(indeces, 6);

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/explode.png");
		m_Shader->SetUniform1i("u_Texture", 0);

	}

	TestTexture2D::~TestTexture2D()
	{
		// Не нужно ничего удалять, т.к. используем умные указатели
	}

	void TestTexture2D::OnUpdate(float deltaTime)
	{
	}

	void TestTexture2D::OnRenderer()
	{
		GLCall(glClearColor(0.0f, 0.0f, 0.0f, 1.0f));
		GLCall(glClear(GL_COLOR_BUFFER_BIT));

		Renderer renderer;

		m_Texture->Bind();

		{

			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}

		{
			glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationB);
			glm::mat4 mvp = m_Proj * m_View * model;
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_MVP", mvp);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
		}
	}

	void TestTexture2D::OnImGuiRenderer()
	{
		ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -2.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		ImGui::SliderFloat3("Translation B", &m_TranslationB.x, -2.0f, 2.0f);
		ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

}