#include "stdafx.h"
#include "VertexBuffer.h"

#include "Renderer.h"

VertexBuffer::VertexBuffer(const void* data, const uint32_t& vertex_size, const uint32_t& buffer_count):
	m_vertex_size(vertex_size), m_buffer_count(buffer_count),
	m_buffer(nullptr), m_layout(nullptr)
{
	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = m_vertex_size * m_buffer_count;
	buff_desc.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = data;

	auto hr = Renderer::GetDevice()->CreateBuffer(&buff_desc, &init_data, &m_buffer);
	assert(SUCCEEDED(hr));
}

void VertexBuffer::SetLayout(D3D11_INPUT_ELEMENT_DESC layout[], const uint32_t& layout_count, const void* shader_byte_code, const size_t& size_byte_shader)
{
	auto hr = Renderer::GetDevice()->CreateInputLayout(layout, layout_count, shader_byte_code, size_byte_shader, &m_layout);
	assert(SUCCEEDED(hr));
}

void VertexBuffer::Bind()
{
	const uint32_t offset = 0;
	Renderer::GetContext()->IASetVertexBuffers(0, 1, m_buffer.GetAddressOf(), &m_vertex_size, &offset);
	Renderer::GetContext()->IASetInputLayout(m_layout.Get());
}

void VertexBuffer::UpdateData(const void* data)
{
	Renderer::GetContext()->UpdateSubresource(m_buffer.Get(), NULL, nullptr, data, NULL, NULL);
}
