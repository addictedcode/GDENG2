#pragma once

class VertexBuffer;
class IndexBuffer;
class Shader;

struct Vertex
{
	float position[3];
	float normal[3];
	float texcoord[2];
	float color[3];
};

struct Loop
{
	Loop() { v = nullptr; prev = this; next = this; }
	
	Vertex* v;
	//radial linked list
	Loop* prev,* next;
};

struct Face
{
	Face() { vertexindices.reserve(3); }
	Face(uint32_t count) { vertexindices.reserve(count); }

	std::vector<uint32_t> vertexindices;
	//float normal[3];
};

enum class MeshPrimitives
{
	NONE, CUBE, PLANE, CYLINDER, CAPSULE, SPHERE
};

class Mesh
{
public:
	Mesh(std::shared_ptr<VertexBuffer>& vertex_buffer, std::shared_ptr<IndexBuffer>& index_buffer);
	Mesh(std::vector<Vertex>&& vertices, std::vector<Face>&& faces);
	~Mesh() = default;
	Mesh operator=(const Mesh & other) = delete;
	Mesh(const Mesh & other) = delete;
	Mesh operator=(Mesh && other) = delete;
	Mesh(Mesh && other) = delete;
public:
	std::vector<Vertex> m_vertices;
	std::vector<Loop> m_loops;
	std::vector<Face> m_faces;
	
	MeshPrimitives m_type = MeshPrimitives::NONE;
private:
	
	std::shared_ptr<VertexBuffer> m_vertex_buffer;
	std::shared_ptr<IndexBuffer> m_index_buffer;

public:
	static std::shared_ptr<Mesh> Create(const MeshPrimitives& type);

	void Bind() const;

	void UpdateBuffers();
	void UpdateVertexBuffer();

	const uint32_t& GetIndexCount() const;
};
