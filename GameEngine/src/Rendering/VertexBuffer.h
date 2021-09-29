#pragma once
#include <d3d11.h>
#include <wrl/client.h>

class VertexBuffer
{
public:
	VertexBuffer(const void* data, const uint32_t& vertex_size, const uint32_t& buffer_count);
	~VertexBuffer() = default;
	VertexBuffer operator=(const VertexBuffer & other) = delete;
	VertexBuffer(const VertexBuffer & other) = delete;
	VertexBuffer operator=(VertexBuffer && other) = delete;
	VertexBuffer(VertexBuffer && other) = delete;
	
private:
	uint32_t m_vertex_size;
	uint32_t m_buffer_count;
	
	Microsoft::WRL::ComPtr<ID3D11Buffer> m_buffer;
	Microsoft::WRL::ComPtr<ID3D11InputLayout> m_layout;

public:
	void SetLayout(D3D11_INPUT_ELEMENT_DESC layout[], const uint32_t& layout_count, const void* shader_byte_code, const size_t& size_byte_shader);

	void Bind();

	void UpdateData(const void* data);

	const uint32_t& GetVertexSize() const { return m_vertex_size; }
	const uint32_t& GetBufferCount() const { return m_buffer_count; }
};

