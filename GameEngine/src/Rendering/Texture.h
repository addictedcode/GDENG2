#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class Texture
{
public:
	Texture(const char* filename);
	~Texture() = default;
	Texture operator=(const Texture & other) = delete;
	Texture(const Texture & other) = delete;
	Texture operator=(Texture && other) = delete;
	Texture(Texture && other) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11ShaderResourceView> m_srv;
	Microsoft::WRL::ComPtr<ID3D11SamplerState> m_ss;
	uint32_t m_width, m_height;

private:
	void CreateSamplerState();

public:
	void Bind();
	
public:
	ID3D11ShaderResourceView* operator*() { return m_srv.Get(); }
	
	uint32_t GetWidth() const { return m_width; }
	uint32_t GetHeight() const { return m_height; }
};
