#include "stdafx.h"
#include "Shader.h"

#include "Renderer.h"

#include <d3dcompiler.h>

Shader::Shader(const wchar_t* file_name):
	m_vertex_shader(nullptr),
	m_pixel_shader(nullptr),
	m_vertex_shader_blob(nullptr)
{
	HRESULT hr;
	
	ID3DBlob* error_blob = nullptr;
	hr = ::D3DCompileFromFile(file_name, nullptr, nullptr, "vsmain", "vs_5_0", 0, 0, &m_vertex_shader_blob, &error_blob);
	assert(SUCCEEDED(hr));
	
	if (error_blob) error_blob->Release();

	hr = Renderer::GetDevice()->CreateVertexShader(m_vertex_shader_blob->GetBufferPointer(), m_vertex_shader_blob->GetBufferSize(), nullptr, &m_vertex_shader);
	assert(SUCCEEDED(hr));

	ID3DBlob* blob = nullptr;

	hr = ::D3DCompileFromFile(file_name, nullptr, nullptr, "psmain", "ps_5_0", 0, 0, &blob, &error_blob);
	assert(SUCCEEDED(hr));

	if (error_blob) error_blob->Release();

	hr = Renderer::GetDevice()->CreatePixelShader(blob->GetBufferPointer(), blob->GetBufferSize(), nullptr, &m_pixel_shader);
	assert(SUCCEEDED(hr));
	
	if (blob) blob->Release();
}

void Shader::Bind() const
{
	Renderer::GetContext()->VSSetShader(m_vertex_shader.Get(), nullptr, 0);
	Renderer::GetContext()->PSSetShader(m_pixel_shader.Get(), nullptr, 0);
}
