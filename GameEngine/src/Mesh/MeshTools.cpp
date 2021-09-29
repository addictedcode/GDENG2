#include "stdafx.h"
#include "MeshTools.h"

#include "tiny_obj_loader/tiny_obj_loader.h"

#include "Mesh.h"


namespace MeshTools {
	
	void InsertLoopToLoop(Loop* main_loop, Loop* new_loop)
	{
		new_loop->prev = main_loop->prev;
		new_loop->prev->next = new_loop;

		new_loop->next = main_loop;
		main_loop->prev = new_loop;
	}

	void GenerateLoops(Mesh* mesh)
	{
		if (mesh->m_vertices.empty()) return;
		auto& vertices = mesh->m_vertices;

		mesh->m_loops.clear();
		mesh->m_loops.resize(vertices.size());
		auto& loops = mesh->m_loops;

		struct pos
		{
			float x, y, z;

			bool operator==(const pos& other) const { return x == other.x && y == other.y && z == other.z; }
		};

		struct hash_fn
		{
			std::size_t operator() (const pos& position) const
			{
				std::size_t h1 = std::hash<float>()(position.x);
				std::size_t h2 = std::hash<float>()(position.y);
				std::size_t h3 = std::hash<float>()(position.z);

				return ((h1 ^ (h2 << 1)) >> 1) ^ (h3 << 1);
			}
		};
		
		std::unordered_map<pos, Loop*, hash_fn> unique_pos;
		unique_pos.reserve(vertices.size());
		
		for (auto i = 0; i < vertices.size(); ++i)
		{
			loops[i].v = &vertices[i];
			pos lpos = { loops[i].v->position[0],loops[i].v->position[1],loops[i].v->position[2] };

			//Loops loops with the same position
			bool is_unique = true;
			if (unique_pos.find(lpos) != unique_pos.end())
			{
				is_unique = false;
				InsertLoopToLoop(unique_pos[lpos], &loops[i]);
			}
			if (is_unique) unique_pos[lpos] = &loops[i];
		}
	}

	std::vector<std::shared_ptr<Mesh>> LoadObjFile(const std::string& filepath)
	{
		tinyobj::ObjReader reader;
		tinyobj::ObjReaderConfig reader_config;
		reader_config.triangulate = false;
		reader_config.vertex_color = true;

		if (!reader.ParseFromFile(filepath, reader_config)) {
			if (!reader.Error().empty()) {
				std::cerr << "TinyObjReader: " << reader.Error();
			}
			assert(false);
		}

		if (!reader.Warning().empty()) {
			std::cout << "TinyObjReader: " << reader.Warning();
		}

		auto& attrib = reader.GetAttrib();
		auto& shapes = reader.GetShapes();
		
		//Reserve # shapes = # meshes
		std::vector<std::shared_ptr<Mesh>> meshes;
		meshes.reserve(shapes.size());
		
		//per shapes
		for (size_t s = 0; s < shapes.size(); s++) {

			//reserve per mesh # indices = # vertices
			std::vector<Vertex> vertices;
			vertices.reserve(shapes[s].mesh.indices.size());

			//reserve per mesh # num_face_vertices = # faces
			std::vector<Face> faces;
			faces.reserve(shapes[s].mesh.num_face_vertices.size());
			
			// Loop over faces(polygon)
			size_t index_offset = 0;
			for (size_t f = 0; f < shapes[s].mesh.num_face_vertices.size(); f++) {
				size_t fv = size_t(shapes[s].mesh.num_face_vertices[f]); //# of vertices per face

				Face face((uint32_t)fv); //Reserve fv for the face

				// Loop over vertices in the face.
				for (size_t v = 0; v < fv; v++) {

					// access to vertex
					tinyobj::index_t idx = shapes[s].mesh.indices[index_offset + v];
					tinyobj::real_t vx = attrib.vertices[3 * size_t(idx.vertex_index) + 0];
					tinyobj::real_t vy = attrib.vertices[3 * size_t(idx.vertex_index) + 1];
					tinyobj::real_t vz = attrib.vertices[3 * size_t(idx.vertex_index) + 2];

					// Check if `normal_index` is zero or positive. negative = no normal data
					tinyobj::real_t nx = 0.f;
					tinyobj::real_t ny = 0.f;
					tinyobj::real_t nz = 0.f;
					if (idx.normal_index >= 0) {
						nx = attrib.normals[3 * size_t(idx.normal_index) + 0];
						ny = attrib.normals[3 * size_t(idx.normal_index) + 1];
						nz = attrib.normals[3 * size_t(idx.normal_index) + 2];
					}

					// Check if `texcoord_index` is zero or positive. negative = no texcoord data
					tinyobj::real_t tx = 0.f;
					tinyobj::real_t ty = 0.f;
					if (idx.texcoord_index >= 0) {
						tx = attrib.texcoords[2 * size_t(idx.texcoord_index) + 0];
						ty = attrib.texcoords[2 * size_t(idx.texcoord_index) + 1];
					}

					// vertex colors
					tinyobj::real_t red   = attrib.colors[3*size_t(idx.vertex_index)+0];
					tinyobj::real_t green = attrib.colors[3*size_t(idx.vertex_index)+1];
					tinyobj::real_t blue  = attrib.colors[3*size_t(idx.vertex_index)+2];

					//Generate vertex
					vertices.push_back({ vx, vy, vz, nx, ny, nz, tx, ty, red, green, blue });
					//vertices.push_back({ vx, vy, vz, nx, ny, nz, tx, ty, 1.f, 1.f, 1.f });
					face.vertexindices.push_back((uint32_t)(index_offset + v));
				}
				index_offset += fv;
				faces.push_back(face);
			}

			//Generate Mesh
			meshes.push_back(std::move(std::make_shared<Mesh>(std::move(vertices), std::move(faces))));
		}
		
		return meshes;
	}
}
