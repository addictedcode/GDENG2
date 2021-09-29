#include "stdafx.h"
#include "Mesh.h"

#include "MeshTools.h"

#include "Rendering/VertexBuffer.h"
#include "Rendering/IndexBuffer.h"
#include "Rendering/Shader.h"

std::shared_ptr<Mesh> Mesh::Create(const MeshPrimitives& type)
{
	switch(type)
	{
	case MeshPrimitives::CUBE:
		{
			static auto meshes = MeshTools::LoadObjFile("res/Meshes/cube.obj");

			std::vector<Vertex> vertices_copy(meshes[0]->m_vertices);
			std::vector<Face> faces_copy(meshes[0]->m_faces);
			auto mesh = std::make_shared<Mesh>(std::move(vertices_copy), std::move(faces_copy));
			mesh->m_type = MeshPrimitives::CUBE;
			return mesh;
		}
	case MeshPrimitives::PLANE:
		{
			static auto meshes = MeshTools::LoadObjFile("res/Meshes/plane.obj");

			std::vector<Vertex> vertices_copy(meshes[0]->m_vertices);
			std::vector<Face> faces_copy(meshes[0]->m_faces);
			auto mesh = std::make_shared<Mesh>(std::move(vertices_copy), std::move(faces_copy));
			mesh->m_type = MeshPrimitives::PLANE;
			return mesh;
		}
	case MeshPrimitives::CYLINDER:
		{
			static auto meshes = MeshTools::LoadObjFile("res/Meshes/cylinder.obj");

			std::vector<Vertex> vertices_copy(meshes[0]->m_vertices);
			std::vector<Face> faces_copy(meshes[0]->m_faces);
			auto mesh = std::make_shared<Mesh>(std::move(vertices_copy), std::move(faces_copy));
			mesh->m_type = MeshPrimitives::CYLINDER;
			return mesh;
		}
	case MeshPrimitives::CAPSULE:
		{
			static auto meshes = MeshTools::LoadObjFile("res/Meshes/capsule.obj");

			std::vector<Vertex> vertices_copy(meshes[0]->m_vertices);
			std::vector<Face> faces_copy(meshes[0]->m_faces);
			auto mesh = std::make_shared<Mesh>(std::move(vertices_copy), std::move(faces_copy));
			mesh->m_type = MeshPrimitives::CAPSULE;
			return mesh;
		}
	case MeshPrimitives::SPHERE:
	{
		static auto meshes = MeshTools::LoadObjFile("res/Meshes/sphere.obj");

		std::vector<Vertex> vertices_copy(meshes[0]->m_vertices);
		std::vector<Face> faces_copy(meshes[0]->m_faces);
		auto mesh = std::make_shared<Mesh>(std::move(vertices_copy), std::move(faces_copy));
		mesh->m_type = MeshPrimitives::SPHERE;
		return mesh;
	}
	}
	assert(false);
	return nullptr;
}

void Mesh::Bind() const
{
	m_vertex_buffer->Bind();
	m_index_buffer->Bind();
}

void Mesh::UpdateBuffers()
{
	MeshTools::GenerateLoops(this);
	
	m_vertex_buffer = std::make_shared<VertexBuffer>(m_vertices.data(), static_cast<uint32_t>(sizeof(Vertex)), static_cast<uint32_t>(m_vertices.size()));

	D3D11_INPUT_ELEMENT_DESC layout[] =
	{
		{"POSITION", 0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 0, D3D11_INPUT_PER_VERTEX_DATA ,0},
		{"NORMAL",   0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 12,D3D11_INPUT_PER_VERTEX_DATA ,0},
		{"TEXCOORD",   0,  DXGI_FORMAT_R32G32_FLOAT, 0, 24,D3D11_INPUT_PER_VERTEX_DATA ,0},
		{"COLOR",    0,  DXGI_FORMAT_R32G32B32_FLOAT, 0, 32,D3D11_INPUT_PER_VERTEX_DATA ,0}
	};

	static std::shared_ptr<Shader> shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
	m_vertex_buffer->SetLayout(layout, 4, shader->GetVertexShaderBlob()->GetBufferPointer(), shader->GetVertexShaderBlob()->GetBufferSize());

	std::vector<uint32_t> indices;

	size_t num_indices = 0;
	for (auto& face : m_faces)
	{
		num_indices += (face.vertexindices.size() - 1) * 3;
	}
	indices.reserve(num_indices);
	
	for (auto& face : m_faces)
	{
		//per triangle
		for (uint64_t i = 1; i < face.vertexindices.size() - 1; ++i)
		{
			indices.push_back(face.vertexindices[0]);
			indices.push_back(face.vertexindices[i]);
			indices.push_back(face.vertexindices[i + 1]);
		}
	}

	m_index_buffer = std::make_shared<IndexBuffer>(indices.data(), (uint32_t)(indices.size()));
}


void Mesh::UpdateVertexBuffer()
{
	m_vertex_buffer->UpdateData(m_vertices.data());
}

const uint32_t& Mesh::GetIndexCount() const
{
	return m_index_buffer->GetCount();
}

Mesh::Mesh(std::shared_ptr<VertexBuffer>& vertex_buffer, std::shared_ptr<IndexBuffer>& index_buffer):
	m_vertex_buffer(std::move(vertex_buffer)), m_index_buffer(std::move(index_buffer))
{
	
}

Mesh::Mesh(std::vector<Vertex>&& vertices, std::vector<Face>&& faces):
	m_vertices(vertices), m_faces(faces)
{
	UpdateBuffers();
}
