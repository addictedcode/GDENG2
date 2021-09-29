#include "stdafx.h"
#include "SceneSerializer.h"

#include "Component.h"
#include "Entity.h"

#include <fstream>
#include <yaml-cpp/yaml.h>

namespace YAML {

	template<>
	struct convert<rp3d::Vector3>
	{
		static Node encode(const rp3d::Vector3& rhs)
		{
			Node node;
			node.push_back(rhs.x);
			node.push_back(rhs.y);
			node.push_back(rhs.z);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, rp3d::Vector3& rhs)
		{
			if (!node.IsSequence() || node.size() != 3)
				return false;

			rhs.x = node[0].as<float>();
			rhs.y = node[1].as<float>();
			rhs.z = node[2].as<float>();
			return true;
		}
	};

	template<>
	struct convert<DirectX::XMVECTOR>
	{
		static Node encode(const DirectX::XMVECTOR& rhs)
		{
			Node node;
			node.push_back(rhs.m128_f32[0]);
			node.push_back(rhs.m128_f32[1]);
			node.push_back(rhs.m128_f32[2]);
			node.push_back(rhs.m128_f32[3]);
			node.SetStyle(EmitterStyle::Flow);
			return node;
		}

		static bool decode(const Node& node, DirectX::XMVECTOR& rhs)
		{
			if (!node.IsSequence() || node.size() != 4)
				return false;

			rhs.m128_f32[0] = node[0].as<float>();
			rhs.m128_f32[1] = node[1].as<float>();
			rhs.m128_f32[2] = node[2].as<float>();
			rhs.m128_f32[3] = node[3].as<float>();
			return true;
		}
	};

}

YAML::Emitter& operator<<(YAML::Emitter& out, const DirectX::XMVECTOR& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.m128_f32[0] << v.m128_f32[1] << v.m128_f32[2] << v.m128_f32[3] << YAML::EndSeq;
	return out;
}

YAML::Emitter& operator<<(YAML::Emitter& out, const rp3d::Vector3& v)
{
	out << YAML::Flow;
	out << YAML::BeginSeq << v.x << v.y << v.z << YAML::EndSeq;
	return out;
}

SceneSerializer::SceneSerializer(const std::shared_ptr<Scene>& scene):
	m_Scene(scene)
{
}

static void SerializeEntity(YAML::Emitter& out, const std::shared_ptr<Entity>& entity)
{
	out << YAML::BeginMap; // Entity
	out << YAML::Key << "Entity" << YAML::Value << entity->GetID();
	out << YAML::Key << "Tag" << YAML::Value << entity->m_tag;

	if (entity->HasComponent<TransformComponent>())
	{
		out << YAML::Key << "TransformComponent";
		out << YAML::BeginMap; // TransformComponent

		auto& tc = entity->GetComponent<TransformComponent>();
		out << YAML::Key << "Translation" << YAML::Value << tc.Translation;
		out << YAML::Key << "Rotation" << YAML::Value << tc.Rotation;
		out << YAML::Key << "Scale" << YAML::Value << tc.Scale;

		out << YAML::EndMap; // TransformComponent
	}

	if (entity->HasComponent<RendererComponent>())
	{
		out << YAML::Key << "RendererComponent";
		out << YAML::BeginMap; // RendererComponent

		auto& rendererComponent = entity->GetComponent<RendererComponent>();
		out << YAML::Key << "Type" << YAML::Value << (int)rendererComponent.mesh->m_type;

		out << YAML::EndMap; // RendererComponent
	}

	if (entity->HasComponent<BoxColliderComponent>())
	{
		out << YAML::Key << "BoxColliderComponent";
		out << YAML::BeginMap; // BoxColliderComponent

		auto& boxColliderComponent = entity->GetComponent<BoxColliderComponent>();
		out << YAML::Key << "Position" << YAML::Value << boxColliderComponent.position;
		out << YAML::Key << "Size" << YAML::Value << boxColliderComponent.size;

		out << YAML::EndMap; // BoxColliderComponent
	}

	if (entity->HasComponent<RigidbodyComponent>())
	{
		out << YAML::Key << "RigidbodyComponent";
		out << YAML::BeginMap; // RigidbodyComponent

		auto& rigidbodyComponent = entity->GetComponent<RigidbodyComponent>();
		out << YAML::Key << "Type" << YAML::Value << (int)rigidbodyComponent.body->getType();
		out << YAML::Key << "IsGravityEnabled" << YAML::Value << rigidbodyComponent.body->isGravityEnabled();

		out << YAML::EndMap; // RigidbodyComponent
	}

	out << YAML::EndMap; // Entity
}

void SceneSerializer::Serialize(const std::string& filepath)
{
	YAML::Emitter out;
	out << YAML::BeginMap;
	out << YAML::Key << "Scene" << YAML::Value << "Untitled";
	out << YAML::Key << "Entities" << YAML::Value << YAML::BeginSeq;
	for (const auto& entity : m_Scene->m_Registry)
	{
		SerializeEntity(out, entity);
	}
	out << YAML::EndSeq;
	out << YAML::EndMap;

	std::ofstream fout(filepath);
	fout << out.c_str();
}

bool SceneSerializer::Deserialize(const std::string& filepath)
{
	YAML::Node data = YAML::LoadFile(filepath);
	if (!data["Scene"])
		return false;

	std::string sceneName = data["Scene"].as<std::string>();

	auto entities = data["Entities"];
	if (entities)
	{
		for (auto entity : entities)
		{
			uint32_t uuid = entity["Entity"].as<uint32_t>(); //Dont use temp
			std::string tag = entity["Tag"].as<std::string>();

			auto deserializedEntity = m_Scene->CreateEntity(uuid, tag);

			auto transformComponent = entity["TransformComponent"];
			if (transformComponent)
			{
				auto& tc = deserializedEntity->AddComponent<TransformComponent>();
				tc.Translation = transformComponent["Translation"].as<DirectX::XMVECTOR>();
				tc.Rotation = transformComponent["Rotation"].as<DirectX::XMVECTOR>();
				tc.Scale = transformComponent["Scale"].as<DirectX::XMVECTOR>();
			}

			auto rendererComponent = entity["RendererComponent"];
			if (rendererComponent)
			{
				auto& rc = deserializedEntity->AddComponent<RendererComponent>();
				rc.mesh = Mesh::Create((MeshPrimitives)rendererComponent["Type"].as<int>());
				rc.shader = std::make_shared<Shader>(L"res/Shaders/BasicShader.hlsl");
			}

			auto boxColliderComponent = entity["BoxColliderComponent"];
			if (boxColliderComponent)
			{
				auto& bcc = deserializedEntity->AddComponent<BoxColliderComponent>(deserializedEntity.get(),
					boxColliderComponent["Position"].as<rp3d::Vector3>(),
					boxColliderComponent["Size"].as<rp3d::Vector3>());
			}

			auto rigidbodyComponent = entity["RigidbodyComponent"];
			if (rigidbodyComponent)
			{
				auto& rbc = deserializedEntity->AddComponent<RigidbodyComponent>(deserializedEntity.get(), m_Scene->GetPhysicsWorld());
				rbc.body->setType((rp3d::BodyType)rigidbodyComponent["Type"].as<int>());
				rbc.body->enableGravity(rigidbodyComponent["IsGravityEnabled"].as<bool>());
			}
		}
	}

	return true;
}


