#pragma once

#include "Test.h"

#include "VertexBuffer.h"
#include "VertexBufferLayout.h"
#include "Texture.h"

#include "glm/glm.hpp"
#include <memory>

namespace test { 

	class TestLightning : public Test
	{
	public:
		TestLightning();
		~TestLightning();

		void OnUpdate(float deltaTime) override;
		void OnRenderer() override;
		void OnImGuiRenderer() override;

	private:
		std::unique_ptr<VertexArray> m_VAO;
		std::unique_ptr<VertexBuffer> m_VertexBuffer;
		std::unique_ptr<IndexBuffer> m_IndexBuffer;	// m_IBO
		std::unique_ptr<Shader> m_Shader;
		std::unique_ptr<Texture> m_Texture;

		glm::mat4 m_Proj, m_View;
		glm::vec3 m_TranslationA, m_TranslationB;

		glm::vec3 m_lightPos0;
		glm::vec3 m_CamPos;

	};

}