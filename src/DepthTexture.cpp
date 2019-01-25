#include "DepthTexture.h"

DepthTexture::DepthTexture()
	: m_RendererID(0),
	m_Width(1024), m_Height(1024)
{
	GLCall(glGenTextures(1, &m_RendererID));
	//glActiveTexture(GL_TEXTURE0 + 1);	// ???
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));

	GLCall(glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT16, m_Width, m_Height, 0, GL_DEPTH_COMPONENT, GL_FLOAT, 0));

	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST));	
	GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST));
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE));	
	//GLCall(glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE));	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	Unbind();
}

DepthTexture::~DepthTexture()
{
	GLCall(glDeleteTextures(1, &m_RendererID));
}

void DepthTexture::Bind(unsigned int slot) const
{
	GLCall(glActiveTexture(GL_TEXTURE0 + slot));
	GLCall(glBindTexture(GL_TEXTURE_2D, m_RendererID));
}

void DepthTexture::Unbind() const
{
	GLCall(glBindTexture(GL_TEXTURE_2D, 0));
}
