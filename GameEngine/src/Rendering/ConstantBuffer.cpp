#include "stdafx.h"
#include "ConstantBuffer.h"

#include "Renderer.h"

ConstantBuffer::ConstantBuffer(const uint32_t& size, const uint32_t& binding):
	m_buffer(nullptr), m_binding(binding)
{
	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = size;
	buff_desc.BindFlags = D3D11_BIND_CONSTANT_BUFFER;
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	auto hr = Renderer::GetDevice()->CreateBuffer(&buff_desc, nullptr, &m_buffer);
	assert(SUCCEEDED(hr));
}

void ConstantBuffer::Bind() const
{
	Renderer::GetContext()->VSSetConstantBuffers(m_binding, 1, m_buffer.GetAddressOf());
	Renderer::GetContext()->PSSetConstantBuffers(m_binding, 1, m_buffer.GetAddressOf());
}

void ConstantBuffer::SetData(const void* data) const
{
	Renderer::GetContext()->UpdateSubresource(m_buffer.Get(), NULL, nullptr, data, NULL, NULL);
	Bind();
}
