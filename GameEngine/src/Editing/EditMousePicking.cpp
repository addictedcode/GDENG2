#include "stdafx.h"
#include "EditMousePicking.h"

#include "Scene/Entity.h"
#include "Mesh/Mesh.h"
#include "Rendering/Shader.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/VertexBuffer.h"
#include "Rendering/FrameBuffer.h"
#include "Rendering/Camera.h"
#include "Rendering/ConstantBuffer.h"
#include "Rendering/Renderer.h"


std::weak_ptr<Entity> EditMousePicking::m_Selected_Entity;
std::unique_ptr<FrameBuffer> EditMousePicking::m_frame_buffer;
std::shared_ptr<VertexBuffer> EditMousePicking::m_faces_index_vb;
std::shared_ptr<IndexBuffer> EditMousePicking::m_faces_index_ib;
std::shared_ptr<Shader> EditMousePicking::m_shader;

struct FaceMousePickingSceneData
{
	DirectX::XMMATRIX model;
	DirectX::XMMATRIX viewprojection;
};

uint32_t EditMousePicking::Begin(const std::shared_ptr<Camera>& camera, std::shared_ptr<Entity> entity, const int x, const int y)
{
	if (m_Selected_Entity.expired()) goto ChangeEntity;
	
	if (*m_Selected_Entity.lock() != *entity)
	{
		ChangeEntity:
		m_Selected_Entity = entity;
	}
	if (!GenerateMesh()) return 0;

	m_frame_buffer->Begin();
	
	static ConstantBuffer constant_buffer(static_cast<uint32_t>(sizeof(FaceMousePickingSceneData)), 0);
	FaceMousePickingSceneData scene_data;
	if (entity->HasComponent<TransformComponent>())
		scene_data.model = entity->GetComponent<TransformComponent>().GetTransform();
	else
		scene_data.model = DirectX::XMMatrixIdentity();
	scene_data.viewprojection = camera->GetViewProjection();
	constant_buffer.SetData(&scene_data);
	
	Renderer::Submit(m_shader, m_faces_index_vb, m_faces_index_ib);
	
	m_frame_buffer->End();

	uint32_t face_index = 0;
	m_frame_buffer->ReadPixel(0, x, y, DXGI_FORMAT_R32_UINT, &face_index);
	
	return face_index;
}

void EditMousePicking::Initialize(const uint32_t width, const uint32_t height)
{
	m_shader = std::make_shared<Shader>(L"res/Shaders/FaceMousePicking.hlsl");
	
	m_frame_buffer = std::make_unique<FrameBuffer>(1, width, height);
	m_frame_buffer->AddBuffer(DXGI_FORMAT_R32_UINT);

	m_frame_buffer->CreateRenderTargetViews();
	m_frame_buffer->CreateDepthStencilView();
}

void EditMousePicking::OnWindowResize(const uint32_t width, const uint32_t height)
{
	m_frame_buffer->Release();
	m_frame_buffer->Resize(width, height);
	m_frame_buffer->CreateRenderTargetViews();
	m_frame_buffer->CreateDepthStencilView();
}

struct EditVertex
{
	float position[3];
	uint32_t index;
};

bool EditMousePicking::GenerateMesh()
{
	auto entity = m_Selected_Entity.lock();

	if (!entity->HasComponent<RendererComponent>()) return false;

	auto& rendererComponent = entity->GetComponent<RendererComponent>();
	auto& mesh = rendererComponent.mesh;
	
	std::vector<EditVertex> vertices;
	vertices.reserve(mesh->m_faces.size() * 4);
	std::vector<uint32_t> indices;
	indices.reserve(mesh->m_faces.size() * 6);
	
	for (auto i = 0; i < mesh->m_faces.size(); ++i)
	{
		auto& face = mesh->m_faces[i];
		for (auto& vertexIndex : face.vertexindices)
		{
			EditVertex vertex;
			for (int i = 0; i < 3; ++i)
				vertex.position[i] = mesh->m_vertices[vertexIndex].position[i];
			vertex.index = i + 1;
			vertices.push_back(vertex);
		}
	}

	m_faces_index_vb = std::make_shared<VertexBuffer>(vertices.data(), static_cast<uint32_t>(sizeof(EditVertex)), static_cast<uint32_t>(vertices.size()));

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA ,0},
		{"FACEINDEX",   0,  DXGI_FORMAT_R32_UINT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA ,0},
	};

	m_faces_index_vb->SetLayout(layout, 2, m_shader->GetVertexShaderBlob()->GetBufferPointer(), m_shader->GetVertexShaderBlob()->GetBufferSize());
	
	uint32_t index = 0;
	for (auto& face : mesh->m_faces)
	{
		//per triangle
		for (auto i = 1; i < face.vertexindices.size() - 1; ++i)
		{
			indices.push_back(index);
			indices.push_back(index+i);
			indices.push_back(index+i+1);
		}
		index += (uint32_t)face.vertexindices.size();
	}

	m_faces_index_ib = std::make_shared<IndexBuffer>(indices.data(), (uint32_t)(indices.size()));
	return true;
}
