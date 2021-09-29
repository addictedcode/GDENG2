#include "stdafx.h"
#include "Renderer.h"

#include "RenderAPI.h"
#include "VertexBuffer.h"
#include "IndexBuffer.h"
#include "ConstantBuffer.h"
#include "Mesh/Mesh.h"
#include "Camera.h"
#include "Scene/Entity.h"
#include "Shader.h"

std::unique_ptr<RenderAPI> Renderer::m_renderAPI = std::make_unique<RenderAPI>();
Renderer::SceneData Renderer::m_scene_data = {};

void Renderer::Initialize(const Window& window)
{
	m_renderAPI->Init(window);
}

void Renderer::BeginScene(const std::shared_ptr<Camera>& camera, const DirectX::XMFLOAT4& light_direction,
	float m_angle)
{
	static ConstantBuffer m_scene_constant_buffer(static_cast<uint32_t>(sizeof(SceneData)), 0);
	m_scene_data.m_angle = m_angle;
	m_scene_data.m_viewprojection = camera->GetViewProjection();
	m_scene_data.m_light_direction = light_direction;
	m_scene_constant_buffer.SetData(&m_scene_data);
}

void Renderer::EndScene()
{

}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<VertexBuffer>& vertex_buffer, const std::shared_ptr<IndexBuffer>& index_buffer)
{
	shader->Bind();
	vertex_buffer->Bind();
	index_buffer->Bind();
	m_renderAPI->Draw(index_buffer->GetCount(), 0, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::Submit(const std::shared_ptr<Shader>& shader, const std::shared_ptr<Mesh>& mesh)
{
	shader->Bind();
	mesh->Bind();
	m_renderAPI->Draw(mesh->GetIndexCount(), 0, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::Submit(const std::shared_ptr<Entity>& entity)
{
	if (!entity->HasComponent<RendererComponent>()) return;
	auto& rendererComponent = entity->GetComponent<RendererComponent>();

	if (!rendererComponent.shader || !rendererComponent.mesh) return;
	rendererComponent.shader->Bind();
	if (rendererComponent.texture) rendererComponent.texture->Bind();
	rendererComponent.mesh->Bind();
	
	struct EntityData
	{
		uint32_t id;
		DirectX::XMMATRIX model;
	};
	static ConstantBuffer m_object_constant_buffer(static_cast<uint32_t>(sizeof(EntityData)), 1);

	EntityData data = {};

	data.id = entity->GetID();
	
	if (entity->HasComponent<TransformComponent>())
		data.model = entity->GetComponent<TransformComponent>().GetTransform();
	else
		data.model = DirectX::XMMatrixIdentity();

	m_object_constant_buffer.SetData(&data);
	m_renderAPI->Draw(rendererComponent.mesh->GetIndexCount(), 0, D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
}

void Renderer::Clear()
{
	m_renderAPI->Clear();
}

void Renderer::SwapBuffer()
{
	m_renderAPI->SwapBuffer();
}

void Renderer::OnWindowResize(const uint32_t& width, const uint32_t& height)
{
	m_renderAPI->OnWindowResize(width, height);
}

const Microsoft::WRL::ComPtr<ID3D11Device>& Renderer::GetDevice()
{
	return m_renderAPI->GetDevice();
}

const Microsoft::WRL::ComPtr<ID3D11DeviceContext>& Renderer::GetContext()
{
	return m_renderAPI->GetContext();
}
