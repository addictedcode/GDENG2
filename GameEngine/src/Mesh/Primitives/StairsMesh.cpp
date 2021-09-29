#include "stdafx.h"
#include "StairsMesh.h"

#include "ImGui/imgui.h"

static std::shared_ptr<Mesh> new_stairs;
static bool is_stairs_new = false;
static int m_steps = 8;
static float size[3] = { 1.f, 1.f, 1.f };

namespace StairsMesh {
	void GenerateStairMesh(std::vector<Vertex>& vertices, std::vector<Face>& faces)
	{
		const uint64_t steps = m_steps;
		const float width = size[0];
		const float height = size[1];
		const float depth = size[2];
		
		const float ex = width * 0.5f;
		const float ey = height * 0.5f;
		const float ez = depth * 0.5f;

		//             |2 quads, 4 vertices each|  |2 Sides|				|Back & Bottom Side|
		vertices.reserve(4 * steps * 2 + (steps * 2 + 2) * 2 + 8);
		faces.reserve(steps * 2 + (2) + 2);

		float inc0, inc1;
		float x0, x1, y0, y1, z0, z1;
		//v : vertex index
		uint64_t v = 0;
		for (auto i = 0; i < steps; ++i, v += 8)
		{
			inc0 = (float)i / (float)steps;
			inc1 = (float)(i + 1) / (float)steps;

			x0 = width - ex;
			x1 = 0.0f - ex;
			y0 = (height * inc0) - ey;
			y1 = (height * inc1) - ey;
			z0 = (depth * inc0) - ez;
			z1 = (depth * inc1) - ez;

			vertices.push_back({ x0,y0,z0		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
			vertices.push_back({ x1,y0,z0		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
			vertices.push_back({ x1,y1,z0		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
			vertices.push_back({ x0,y1,z0		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });

			vertices.push_back({ x1,y1,z1		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
			vertices.push_back({ x0,y1,z1		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
			vertices.push_back({ x0,y1,z0		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
			vertices.push_back({ x1,y1,z0		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });

			{
				Face face(4);
				face.vertexindices.insert(face.vertexindices.end(), { (uint32_t)v + 0, (uint32_t)v + 1, (uint32_t)v + 2, (uint32_t)v + 3 });
				faces.push_back(face);
			}
			{
				Face face(4);
				face.vertexindices.insert(face.vertexindices.end(), { (uint32_t)v + 4, (uint32_t)v + 5, (uint32_t)v + 6, (uint32_t)v + 7 });
				faces.push_back(face);
			}

		}


		float x = 0.f;

		for (int side = 0; side < 2; side++)
		{
			Face side_face((uint32_t)steps * 2 + 2);

			side % 2 == 0 ?
				vertices.push_back({ 0.f - ex,0.f - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 }) :
				vertices.push_back({ width - ex,0.f - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });

			side_face.vertexindices.push_back((uint32_t)v);
			v += 1;


			for (int i = 0; i < steps; i++)
			{
				inc0 = i / (float)steps;
				inc1 = (i + 1) / (float)steps;

				y0 = inc0 * height;
				y1 = inc1 * height;

				inc0 = i / (float)steps;

				z0 = inc0 * depth;
				z1 = inc1 * depth;

				vertices.push_back({ x - ex,y0 - ey,z0 - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
				vertices.push_back({ x - ex,y1 - ey,z0 - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });

				side % 2 == 0 ?
					side_face.vertexindices.insert(side_face.vertexindices.begin() + 1, { (uint32_t)v + 1, (uint32_t)v + 0 }) :
					side_face.vertexindices.insert(side_face.vertexindices.end(), { (uint32_t)v + 0, (uint32_t)v + 1 });

				v += 2;
			}
			if (side % 2 == 0) {
				vertices.push_back({ 0.f - ex,height - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
				side_face.vertexindices.insert(side_face.vertexindices.begin() + 1, (uint32_t)v);
			}
			else {
				vertices.push_back({ width - ex,height - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
				side_face.vertexindices.push_back((uint32_t)v);
			}
			v += 1;

			faces.push_back(side_face);
			x += width;
		}

		// add that back face
		vertices.push_back({ 0.f - ex,0.f - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
		vertices.push_back({ width - ex,0.f - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
		vertices.push_back({ width - ex,height - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
		vertices.push_back({ 0.f - ex,height - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });

		{
			Face face(4);
			face.vertexindices.insert(face.vertexindices.end(), { (uint32_t)v + 0, (uint32_t)v + 1, (uint32_t)v + 2, (uint32_t)v + 3 });
			faces.push_back(face);
		}
		v += 4;

		// add that bottom face
		vertices.push_back({ 0.f - ex,0.f - ey,0.f - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
		vertices.push_back({ width - ex,0.f - ey,0.f - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
		vertices.push_back({ width - ex,0.f - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });
		vertices.push_back({ 0.f - ex,0.f - ey,depth - ez		,0.0f,0.0f,0.0f,0.0f,0.0f,1,1,1 });

		{
			Face face(4);
			face.vertexindices.insert(face.vertexindices.end(), { (uint32_t)v + 0, (uint32_t)v + 1, (uint32_t)v + 2, (uint32_t)v + 3 });
			faces.push_back(face);
		}
	}
	
	std::shared_ptr<Mesh> StartStairMesh()
	{
		std::vector<Vertex> vertices;
		std::vector<Face> faces;
		GenerateStairMesh(vertices, faces);
		new_stairs = std::make_shared<Mesh>(std::move(vertices), std::move(faces));
		is_stairs_new = true;
		return new_stairs;
	}


	void StairMeshPanel()
	{
		if (is_stairs_new)
		{
			static bool opened = true;
			if (opened) ImGui::SetNextWindowFocus();
			ImGui::Begin("Stairs", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoDocking | ImGuiWindowFlags_NoResize);
			
			if (opened) opened = false;
			if (!ImGui::IsWindowFocused()) {
				is_stairs_new = false;
				opened = true;
			}
			
			bool has_edited = false;
			if (ImGui::SliderInt("Steps", &m_steps, 2, 64)) has_edited = true;
			if (ImGui::DragFloat3("Size", size, 0.1f, 0.1f, 256.f, "%.2f")) has_edited = true;

			std::vector<Vertex> vertices;
			std::vector<Face> faces;
			GenerateStairMesh(vertices, faces);

			new_stairs->m_vertices = std::move(vertices);
			new_stairs->m_faces = std::move(faces);
			new_stairs->UpdateBuffers();
			
			ImGui::End();
		}
	}
}
