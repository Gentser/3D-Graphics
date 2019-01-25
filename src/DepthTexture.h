#pragma once

#include "Renderer.h"

class DepthTexture
{
private:
	unsigned int m_RendererID;
	int m_Width, m_Height;	// BPP = bits per pixel
public:
	DepthTexture();
	~DepthTexture();

	void Bind(unsigned int slot = 0) const;
	void Unbind() const;

	inline int GetWidth() const { return m_Width; }
	inline int GetHeight() const { return m_Height; }
	inline unsigned int GetId() const { return m_RendererID; }
};