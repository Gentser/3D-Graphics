#include "Test3DModel.h"

#include "Renderer.h"
#include "ObjLoader.h"

#include "imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <malloc.h>

namespace test {

	Test3DModel::Test3DModel(GLFWwindow* window)
		: m_Proj(glm::ortho(-2.0f, 2.0f, -1.5f, 1.5f, -1.0f, 100.0f)),
		m_TranslationA(0.0f, 0.5f, 1.0f), m_lightPos0(glm::vec3(0.f, 0.f, -2.f)),
		m_horizontalAngle(0.0f), m_verticalAngle(0.0f), m_FoV(45.0f),
		m_window(window), m_position(glm::vec3(0, 0, -5)),
		m_speed(3.f), m_mouseSpeed(0.05f), debugMode(false)

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

		float* vertData = NULL;
		unsigned int* inds = NULL;
		int vSize, iSize;

		ObjLoader::loadObjModel("res/models/TexCube.obj", m_vertices, m_indData);
		
		/*if (vertData == nullptr)
			std::cout << "ERROR! vertData == nullptr" << std::endl;
		if (inds == nullptr)
			std::cout << "ERROR! inds == nullptr" << std::endl;

		for (int i = 0; i < m_vertices.size(); i++)
		{
			if (i % 8 == 0 && i != 0) std::cout << std::endl;
			std::cout << " " << m_vertices[i];
		}
		for (int i = 0; i < m_indData.size(); i++)
		{
			std::cout << " " << m_indData[i];
		} std::cout << std::endl; */


		m_VAO = std::make_unique<VertexArray>();
		vertData = &m_vertices[0];
		m_VertexBuffer = std::make_unique<VertexBuffer>(vertData, m_vertices.size() * sizeof(m_vertices[0]));

		VertexBufferLayout layout;
		layout.Push<float>(3);	// Positions
		layout.Push<float>(2);	// Text. coords
		layout.Push<float>(3);	// Normals

		m_VAO->AddBuffer(*m_VertexBuffer, layout);
		inds = &m_indData[0];
		m_IndexBuffer = std::make_unique<IndexBuffer>(inds, m_indData.size());

		m_Shader = std::make_unique<Shader>("res/shaders/Basic.shader");
		m_Shader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/marble16.jpg");
		m_Shader->SetUniform1i("u_Texture", 0);

		m_lastTime = glfwGetTime();

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_CULL_FACE));

		if (!debugMode)
			glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	}

	Test3DModel::~Test3DModel()
	{
		m_window = nullptr;
		// Не нужно ничего удалять, т.к. используем умные указатели
	}

	void Test3DModel::OnUpdate(float deltaTime)
	{		
		computeMatricesFromInputs(deltaTime);
	}

	void Test3DModel::OnRenderer()
	{
		Renderer renderer;
		renderer.Clear();

		m_Shader->Bind();
		m_Shader->SetUniform3f("u_LightPos0", m_lightPos0[0], m_lightPos0[1], m_lightPos0[2]);

		m_Texture->Bind();

		glm::mat4 model = glm::translate(glm::mat4(1.0f), m_TranslationA);
		{		
			m_Shader->Bind();
			m_Shader->SetUniformMat4f("u_ModelMatrix", model);
			m_Shader->SetUniformMat4f("u_ViewMatrix", m_View);
			m_Shader->SetUniformMat4f("u_ProjMatrix", m_Proj);
			m_Shader->SetUniform3f("u_CamPose", m_position[0], m_position[1], m_position[2]);

			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_Shader);
			m_Shader->Unbind();
		}

		// Debug mode
		if (debugMode)
		{	
			GLCall(glMatrixMode(GL_PROJECTION));
			GLCall(glLoadMatrixf((const GLfloat*)&m_Proj[0]));
			GLCall(glMatrixMode(GL_MODELVIEW));
			glm::mat4 MV = m_View * model;
			GLCall(glLoadMatrixf((const GLfloat*)&MV[0]));

			GLCall(glColor3f(1, 0, 0));
			glBegin(GL_LINES);
			for (int i = 0; i < m_indData.size(); i++)
			{
				glm::vec3 p;
				p.x = m_vertices[m_indData[i] * 8 + 0];
				p.y = m_vertices[m_indData[i] * 8 + 1];
				p.z = m_vertices[m_indData[i] * 8 + 2];
				glVertex3fv(&p.x);

				glm::vec3 o;
				o.x = m_vertices[m_indData[i] * 8 + 5];
				o.y = m_vertices[m_indData[i] * 8 + 6];
				o.z = m_vertices[m_indData[i] * 8 + 7];
				o = glm::normalize(o);
				p += o * 0.1f;
				glVertex3fv(&p.x);
			}
			glEnd();

			GLCall(glColor3f(1, 0, 1));
			glBegin(GL_LINES);
			glVertex3f(m_lightPos0.x, m_lightPos0.y + 0.05, m_lightPos0.z);
			glVertex3f(m_lightPos0.x, m_lightPos0.y - 0.05, m_lightPos0.z);
			glVertex3f(m_lightPos0.x - 0.05, m_lightPos0.y, m_lightPos0.z);
			glVertex3f(m_lightPos0.x + 0.05, m_lightPos0.y, m_lightPos0.z);
			glVertex3f(m_lightPos0.x, m_lightPos0.y, m_lightPos0.z - 0.05);
			glVertex3f(m_lightPos0.x, m_lightPos0.y, m_lightPos0.z + 0.05);
			glEnd();
		}
		
	}

	void Test3DModel::OnImGuiRenderer()
	{
		//ImGui::SliderFloat3("Translation A", &m_TranslationA.x, -2.0f, 2.0f);            // Edit 1 float using a slider from 0.0f to 1.0f    
		//ImGui::Text("Application average %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
	}

	void Test3DModel::computeMatricesFromInputs(float deltaTime)
	{
		//					ORIENTATION
		double xpos, ypos;
		int w_height, w_width;

		//deltaTime = float(glfwGetTime() - m_lastTime);
		glfwGetWindowSize(m_window, &w_width, &w_height);
		glfwGetCursorPos(m_window, &xpos, &ypos);

		m_horizontalAngle += m_mouseSpeed * deltaTime * float(w_width / 2 - xpos);
		m_verticalAngle += m_mouseSpeed * deltaTime * float(w_height / 2 - ypos);

		// Direction : Spherical coordinates to Cartesian coordinates conversion
		glm::vec3 direction(
			cos(m_verticalAngle) * sin(m_horizontalAngle),
			sin(m_verticalAngle),
			cos(m_verticalAngle) * cos(m_horizontalAngle)
		);

		// Right vector
		glm::vec3 right(
			sin(m_horizontalAngle - 3.14f / 2.0f),
			0,
			cos(m_horizontalAngle - 3.14f / 2.0f)
		);

		// Up vector : perpendicular to both direction and right
		glm::vec3 up = glm::cross(right, direction);


		//					POSITION
		// Move forward
		if (glfwGetKey(m_window, GLFW_KEY_W) == GLFW_PRESS) {
			m_position += direction * deltaTime * m_speed;
		}
		// Move backward
		if (glfwGetKey(m_window, GLFW_KEY_S) == GLFW_PRESS) {
			m_position -= direction * deltaTime * m_speed;
		}
		// Move right
		if (glfwGetKey(m_window, GLFW_KEY_D) == GLFW_PRESS) {
			m_position += right * deltaTime * m_speed;
		}
		// Move left
		if (glfwGetKey(m_window, GLFW_KEY_A) == GLFW_PRESS) {
			m_position -= right * deltaTime * m_speed;
		}
		// Move left
		if (glfwGetKey(m_window, GLFW_KEY_SPACE) == GLFW_PRESS) {
			m_position += glm::vec3(0.f, 1.f, 0.f) * deltaTime * m_speed;
		}
		// Move left
		if (glfwGetKey(m_window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
			m_position -= glm::vec3(0.f, 1.f, 0.f) * deltaTime * m_speed;
		}

		//					COMPUTONG THE MATRICES
		// Projection matrix : 45&deg; Field of View, 4:3 ratio, display range : 0.1 unit <-> 100 units
		m_Proj = glm::perspective(glm::radians(m_FoV), 4.f / 3.f, 0.1f, 100.f);

		// Camera matrix
		m_View = glm::lookAt(
			m_position,				// Camera is here
			m_position + direction,	// and looks here : at the same position, plus "direction"
			up						// Head is up (set to 0,-1,0 to look upside-down)
		);


		glfwSetCursorPos(m_window, w_width / 2, w_height / 2);
	}
}