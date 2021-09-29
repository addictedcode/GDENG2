#include "stdafx.h"
#include "IndexBuffer.h"

#include "Renderer.h"

IndexBuffer::IndexBuffer(const void* data, const uint32_t& count):
	m_count(count),
	m_buffer(nullptr)
{
	D3D11_BUFFER_DESC buff_desc = {};
	buff_desc.Usage = D3D11_USAGE_DEFAULT;
	buff_desc.ByteWidth = sizeof(uint32_t) * m_count;
	buff_desc.BindFlags = D3D11_BIND_INDEX_BUFFER;
	buff_desc.CPUAccessFlags = 0;
	buff_desc.MiscFlags = 0;

	D3D11_SUBRESOURCE_DATA init_data = {};
	init_data.pSysMem = data;

	auto hr = Renderer::GetDevice()->CreateBuffer(&buff_desc, &init_data, &m_buffer);
	assert(SUCCEEDED(hr));
}

void IndexBuffer::Bind() const
{
	Renderer::GetContext()->IASetIndexBuffer(m_buffer.Get(), DXGI_FORMAT_R32_UINT, 0);
}
