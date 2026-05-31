#include "GameObject.hh"
#include "Globals.hh"
#include "raymath.h"

#include <filesystem>
#include <format>
#include <raylib.h>

namespace BrrEdit {
	Room::Room() : m_Objs({}) {}

	void Room::Render() {
		for (GameObject& obj : m_Objs) {
			auto pos = std::bit_cast<Vector3>(obj.ObjTransform.Pos);
			auto scale = std::bit_cast<Vector3>(obj.ObjTransform.Scale);
			auto rot = std::bit_cast<Vector3>(obj.ObjTransform.Rot);

			if (Object* object = std::get_if<Object>(&obj.Obj)) {
				auto color = Color { object->Col.X, object->Col.Y, object->Col.Z, 255 };

				if (g_Wireframe) {
					DrawCubeWiresV(
						pos,
						scale,
						color
					);
				} else {
					DrawCubeV(
						pos,
						scale,
						color
					);
				}
			} else if (Mesh* mesh = std::get_if<Mesh>(&obj.Obj)) {
				auto color = Color { mesh->Col.X, mesh->Col.Y, mesh->Col.Z, 255 };
				auto meshId = mesh->MeshId;

				auto path = std::format("./{}.glb", meshId);

				if (!std::filesystem::exists(path)) {
					continue;
				}

				char* path_c = const_cast<char*>(path.c_str());

				auto& model = g_Models[path_c];

				if (model.meshCount == 0) {
					model = LoadModel(path_c);

					for (int i = 0; i < model.materialCount; i++) {
						// move map 1 to map 0 (albedo)
						if (model.materials[i].maps[1].texture.id > 1) {
							model.materials[i].maps[MATERIAL_MAP_ALBEDO] = model.materials[i].maps[1];
						}
						model.materials[i].shader = g_PbrShader;
					}

					for (int i = 0; i < model.materialCount; i++) {
						for (int j = 0; j < 12; j++) {
							if (model.materials[i].maps[j].texture.id > 1) {
								printf("mat %d map %d tex %d %dx%d\n", i, j, 
										model.materials[i].maps[j].texture.id,
										model.materials[i].maps[j].texture.width,
										model.materials[i].maps[j].texture.height);
							}
						}
					}

					for (int i = 0; i < model.materialCount; i++) {
						model.materials[i].shader = g_PbrShader;
					}
				}

				Matrix transform = 
					MatrixScale(scale.x, scale.y, scale.z) *
					MatrixTranslate(pos.x, pos.y, pos.z) *
					MatrixRotateXYZ(rot);

				model.transform = transform;

				float albedoColor[4] = { color.r/255.0f, color.g/255.0f, color.b/255.0f, 1.0f };
				SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "albedoColor"), albedoColor, SHADER_UNIFORM_VEC4);

			if (g_Wireframe) {
				DrawModelWires(
					model,
					{0,0,0},
					1.0f,
					color
				);
			} else {
				DrawModel(
					model,
					{0,0,0},
					1.0f,
					color
				);
			}
			}
		}
	}

	void Room::AddMesh() {
		m_Selected = m_Objs.size();

		m_Objs.emplace_back(
			m_Objs.size(),
			"Unnamed Mesh",
			Transform::Default(),
			std::variant<Mesh, Sound, Object>{ Mesh(Vec3b::Max(), "model") }
		);
	}

	void Room::AddObject() {
		m_Selected = m_Objs.size();

		m_Objs.emplace_back(
			m_Objs.size(),
			"Unnamed Object",
			Transform::Default(),
			std::variant<Mesh, Sound, Object>{ Object { .Col = Vec3b::Max() } }
		);
	}

	Room::~Room() {}

	GameObject* Room::GetSelected() {
		auto it = std::ranges::find_if(
			m_Objs.begin(),
			m_Objs.end(),
			[this](const GameObject& obj) {
				return obj.Id == m_Selected;
			}
		);

		if (it != m_Objs.end()) {
			return it.base();
		}

		return nullptr;
	}
}
