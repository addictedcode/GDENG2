#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class Shader
{
public:
	Shader(const wchar_t* file_name);
	~Shader() = default;
	Shader operator=(const Shader& other) = delete;
	Shader(const Shader& other) = delete;
	Shader operator=(Shader&& other) = delete;
	Shader(Shader&& other) = delete;

private:
	Microsoft::WRL::ComPtr<ID3D11VertexShader> m_vertex_shader;
	Microsoft::WRL::ComPtr<ID3D11PixelShader> m_pixel_shader;

	Microsoft::WRL::ComPtr<ID3DBlob> m_vertex_shader_blob;

public:
	void Bind() const;
	
	Microsoft::WRL::ComPtr<ID3DBlob> GetVertexShaderBlob() const { return m_vertex_shader_blob; }
};