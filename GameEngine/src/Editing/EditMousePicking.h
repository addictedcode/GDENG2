#pragma once

class Entity;
class Camera;
class FrameBuffer;
class VertexBuffer;
class IndexBuffer;
class Shader;

class EditMousePicking
{
private:
	static std::unique_ptr<FrameBuffer> m_frame_buffer;
	static std::weak_ptr<Entity> m_Selected_Entity;
	static std::shared_ptr<VertexBuffer> m_faces_index_vb;
	static std::shared_ptr<IndexBuffer> m_faces_index_ib;
	static std::shared_ptr<Shader> m_shader;

public:
	static uint32_t Begin(const std::shared_ptr<Camera>& camera, std::shared_ptr<Entity> entity, const int x, const int y);

	static void Initialize(const uint32_t width, const uint32_t height);
	static void OnWindowResize(const uint32_t width, const uint32_t height);

private:

	static bool GenerateMesh();
};
