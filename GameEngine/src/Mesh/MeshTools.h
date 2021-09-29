#pragma once

struct Loop;
class Mesh;

namespace MeshTools
{
	void InsertLoopToLoop(Loop* main_loop, Loop* new_loop);

	void GenerateLoops(Mesh* mesh);

	std::vector<std::shared_ptr<Mesh>> LoadObjFile(const std::string& filepath);
}
