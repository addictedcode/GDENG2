#pragma once

#include "Rendering/Camera.h"
#include "Rendering/Shader.h"
#include "Entity.h"
#include <reactphysics3d/reactphysics3d.h>

class Scene
{
public:
	Scene();
	~Scene() = default;
	Scene operator=(const Scene & other) = delete;
	Scene(const Scene & other) = delete;
	Scene operator=(Scene && other) = delete;
	Scene(Scene && other) = delete;

private:
	std::shared_ptr<Camera> m_Scene_Camera;
	std::shared_ptr<rp3d::PhysicsWorld> m_physics_world;

	std::unordered_set<std::shared_ptr<Entity>> m_Registry;
	
public:
	std::shared_ptr<Entity> CreateEntity();
	std::shared_ptr<Entity> CreateEntity(const uint32_t& id, const std::string& tag);
	void AddEntity(Entity&& entity);
	void DestroyEntity(const std::shared_ptr<Entity>& entity);

	void OnUpdate(float delta_time);
	void OnFixedUpdate(float timestep);

	const std::shared_ptr<Camera>& GetSceneCamera() const { return m_Scene_Camera; }
	std::shared_ptr<rp3d::PhysicsWorld>& GetPhysicsWorld() { return m_physics_world; }

private:
	friend class SceneHierarchyPanel;
	friend class SceneViewport;
	friend class SceneSerializer;
};