#include "Test—oursework.h"

#include "Renderer.h"
#include "ObjLoader.h"

#include "imgui/imgui.h"
#include "glm/gtc/matrix_transform.hpp"

#include <iostream>
#include <malloc.h>

namespace test {

	Test—oursework::Test—oursework(GLFWwindow* window)
		: m_lightPos0(glm::vec3(2.f, 5.f, -5.f)), m_position(glm::vec3(2, 3, -5)),
		m_horizontalAngle(0.0f), m_verticalAngle(0.0f), m_FoV(45.0f),
		m_speed(3.f), m_mouseSpeed(0.05f), m_window(window),
		m_Proj(glm::perspective(glm::radians(m_FoV), 4.f / 3.f, 0.1f, 100.f)),
		debugMode(false)

	{
		float* vertData = NULL;
		unsigned int* inds = NULL;
		int vSize, iSize;

		ObjLoader::loadObjModel("res/models/CourseWork1.obj", m_vertices, m_indData);	// CourseWork1

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

		m_BasicShader = std::make_unique<Shader>("res/shaders/Test3DLightMap_base.shader");
		m_BasicShader->Bind();

		m_Texture = std::make_unique<Texture>("res/textures/CourseWork.png");	// CourseWork
		m_Texture->Bind(0);
		m_BasicShader->SetUniform1i("u_Texture", 0);
		

		//			LIGHT MAP
			GLCall(glGenFramebuffers(1, &m_FramebufferName ));
			GLCall(glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName));

			m_DepthShader = std::make_unique<Shader>("res/shaders/Test3DLightMap_depth.shader");

			m_DepthTexture = std::make_unique<DepthTexture>();
			m_DepthTexture->Bind(1);
			m_BasicShader->SetUniform1i("u_depthTexture", 1);

			// Set "depthTexture" as our colour attachement
			GLCall(glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_DepthTexture->GetId(), 0));
			GLCall(glDrawBuffer(GL_NONE)); // No color buffer is drawn to.

			GLCall(if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
				std::cout << " ERROR: !GL_FRAMEBUFFER_COMPLETE " << std::endl);
			
			// ›ÙÙÂÍÚ œËÚÂ‡ œÂÌ‡ - glCullFace(GL_FRONT) ÔË ËÒÓ‚‡ÌËË Í‡Ú˚ ÚÂÌÂÈ, glCullFace(GL_FRONT) ÔË ÂÌ‰ÂËÌ„Â ÒˆÂÌ˚ 
		// -----------------------------------

		glfwSetInputMode(m_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		GLCall(glEnable(GL_BLEND));
		GLCall(glEnable(GL_DEPTH_TEST));
		GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));
		GLCall(glEnable(GL_CULL_FACE));
	}

	Test—oursework::~Test—oursework()
	{
		m_window = nullptr;
		// ÕÂ ÌÛÊÌÓ ÌË˜Â„Ó Û‰‡ÎˇÚ¸, Ú.Í. ËÒÔÓÎ¸ÁÛÂÏ ÛÏÌ˚Â ÛÍ‡Á‡ÚÂÎË
	}

	void Test—oursework::OnUpdate(float deltaTime)
	{		
		computeMatricesFromInputs(deltaTime);
	}

	void Test—oursework::OnRenderer()
	{
		Renderer renderer;
		renderer.Clear();
		int wWidth, wHeight;
		glfwGetWindowSize(m_window, &wWidth, &wHeight);
		
		glm::mat4 model = glm::mat4(1.0f);
		{		
			//			LIGHT MAP
			// Compute the MVP matrix from the light's point of view
			m_lightPos0 = m_position;
			glm::mat4 depthProjectionMatrix = glm::ortho<float>(-10.f, 10.f, -10.f, 10.f, -10.f, 20.f);
			glm::mat4 depthViewMatrix = glm::lookAt(m_lightPos0, glm::vec3(0, 0, 0), glm::vec3(0, 1, 0));
			glm::mat4 depthModelMatrix = glm::mat4(1.0);
			//glm::mat4 depthMVP = depthProjectionMatrix * depthViewMatrix * depthModelMatrix;
			glm::mat4 depthMVP = depthProjectionMatrix * m_View * model;

			m_DepthShader->Bind();
			m_DepthShader->SetUniformMat4f("u_DepthMVP", depthMVP);

			glViewport(0, 0, m_DepthTexture->GetWidth(), m_DepthTexture->GetHeight());
			glBindFramebuffer(GL_FRAMEBUFFER, m_FramebufferName );
			renderer.Clear();
			GLCall(glCullFace(GL_FRONT));
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_DepthShader);
			glBindFramebuffer(GL_FRAMEBUFFER, 0);
			m_DepthShader->Unbind();

			// -----------------

			m_BasicShader->Bind();

			glm::mat4 biasMatrix(
				0.5f, 0.0f, 0.0f, 0.0f,
				0.0f, 0.5f, 0.0f, 0.0f,
				0.0f, 0.0f, 0.5f, 0.0f,
				0.5f, 0.5f, 0.5f, 1.0f
			);
			glm::mat4 depthBiasMVP = biasMatrix * depthMVP;
			m_BasicShader->SetUniformMat4f("u_DepthBiasMVP", depthBiasMVP);


			m_BasicShader->SetUniform3f("u_LightPos0", m_lightPos0[0], m_lightPos0[1], m_lightPos0[2]);
			m_BasicShader->SetUniformMat4f("u_ModelMatrix", model);
			m_BasicShader->SetUniformMat4f("u_ViewMatrix", m_View);
			m_BasicShader->SetUniformMat4f("u_ProjMatrix", m_Proj);
			m_BasicShader->SetUniform3f("u_CamPose", m_position[0], m_position[1], m_position[2]);

			m_Texture->Bind();

			glViewport(0, 0, wWidth, wHeight);
			GLCall(glCullFace(GL_BACK));
			renderer.Draw(*m_VAO, *m_IndexBuffer, *m_BasicShader);
			m_BasicShader->Unbind();
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

			// Light position
			{
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

			// Begining of the worldspace
			{
				GLCall(glColor3f(0, 1, 0));
				glBegin(GL_LINES);
				glVertex3f(0.f, 0.f + 0.05, 0.f);
				glVertex3f(0.f, 0.f - 0.05, 0.f);
				glVertex3f(0.f - 0.05, 0.f, 0.f);
				glVertex3f(0.f + 0.05, 0.f, 0.f);
				glVertex3f(0.f, 0.f, 0.f - 0.05);
				glVertex3f(0.f, 0.f, 0.f + 0.05);
				glEnd();
			}


		}
		
	}

	void Test—oursework::OnImGuiRenderer()
	{
		
	}

	void Test—oursework::computeMatricesFromInputs(float deltaTime)
	{
		//					ORIENTATION
		double xpos, ypos;
		int w_height, w_width;

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