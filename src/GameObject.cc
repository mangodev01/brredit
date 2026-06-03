#include "GameObject.hh"

#include "File.hh"
#include "Globals.hh"
#include "raymath.h"

#include <algorithm>
#include <filesystem>
#include <format>

#include <print>
#include <rlgl.h>
#include <variant>

#include <pfd.h>

namespace BrrEdit {
	Room::Room() : m_Objs({}) {}

	Room::~Room() {
		for (auto& [id, model] : g_Models) {
			UnloadModel(model);
		}

		for (auto& [id, music] : g_Sounds) {
			UnloadMusicStream(*music);
		}
	}

	void Room::Render() {
		for (GameObject& obj : m_Objs) {
			auto pos = std::bit_cast<Vector3>(obj.ObjTransform.Pos);
			auto scale = std::bit_cast<Vector3>(obj.ObjTransform.Scale);
			auto rot = std::bit_cast<Vector3>(obj.ObjTransform.Rot);


			Matrix transform =
				MatrixScale(scale.x, scale.y, scale.z) *
				MatrixTranslate(pos.x, pos.y, pos.z) *
				MatrixRotateXYZ(rot);

			rlPushMatrix();
			rlMultMatrixf(MatrixToFloat(transform));

			if (Object* object = std::get_if<Object>(&obj.Obj)) {
				auto color = Color { object->Col.X, object->Col.Y, object->Col.Z, 255 };

				if (g_Wireframe) {
					DrawCubeWiresV({0,0,0}, {1,1,1}, color);
				} else {
					DrawCubeV({0,0,0}, {1,1,1}, color);
				}

				if (obj.Id == m_Selected) {
					DrawCubeWiresV({0,0,0}, {1.02,1.02,1.02}, SKYBLUE);
				}
			} else if (Mesh* mesh = std::get_if<Mesh>(&obj.Obj)) {
				auto color = Color { mesh->Col.X, mesh->Col.Y, mesh->Col.Z, 255 };
				auto path_str = std::format("./{}.glb", mesh->MeshId);
				auto path = const_cast<char*>(path_str.c_str());

				if (!std::filesystem::exists(path)) {
					rlPopMatrix();
					continue;
				}

				auto& model = g_Models[path];

				if (model.meshCount == 0) {
					model = LoadModel(path);
				}

				model.transform = transform;

				if (g_Wireframe) {
					DrawModelWires(model, {0,0,0}, 1.0f, color);
				} else {
					DrawModel(model, {0,0,0}, 1.0f, color);
				}
			} else if (Sound* sound = std::get_if<Sound>(&obj.Obj)) {
				DrawCubeV({0,0,0}, {1,1,1}, { 255, 0, 0, 64 });
			}

			rlPopMatrix();
		}
	}

	void Room::Update(Camera3D cam) {
		for (GameObject& obj : m_Objs) {
			if (Sound* sound = std::get_if<Sound>(&obj.Obj)) {
				auto path_str = std::format("./{}.ogg", sound->SoundId);
				auto path = const_cast<char*>(path_str.c_str());

				if (!std::filesystem::exists(path)) {
					continue;
				}

				std::unique_ptr<Music>& music = g_Sounds[path];

				bool shouldPlay = PointInOBB(std::bit_cast<Vec3f>(cam.position), obj.ObjTransform);

				if (!music) {
					music = std::make_unique<Music>(LoadMusicStream(path_str.c_str()));

				}

				if (shouldPlay) {
					if (!IsMusicStreamPlaying(*music))
						PlayMusicStream(*music);
				} else {
					StopMusicStream(*music);
				}

				if (shouldPlay) {
					UpdateMusicStream(*music);
				}
			}
		}
	}

	void Room::AddMesh() {
		m_Selected = m_Objs.size();
		char name[64];
		strcpy(name, "Mesh ");
		strcat(name, std::to_string(m_Objs.size()).c_str());
		m_Objs.emplace_back(
			m_Objs.size(),
			name,
			Transform::Default(),
			std::variant<Mesh, Sound, Object>{ Mesh(Vec3b::Max(), "model") }
		);
	}

	void Room::AddObject() {
		m_Selected = m_Objs.size();
		char name[64];
		strcpy(name, "Object ");
		strcat(name, std::to_string(m_Objs.size()).c_str());
		m_Objs.emplace_back(
			m_Objs.size(),
			name,
			Transform::Default(),
			std::variant<Mesh, Sound, Object>{ Object { .Col = Vec3b::Max() } }
		);
	}

	void Room::AddSound() {
		m_Selected = m_Objs.size();
		char name[64];
		strcpy(name, "Sound ");
		strcat(name, std::to_string(m_Objs.size()).c_str());
		m_Objs.emplace_back(
			m_Objs.size(),
			name,
			Transform::Default(),
			std::variant<Mesh, Sound, Object>{ Sound("sound") }
		);
	}

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

	void Room::Select(uint32_t id) {
		m_Selected = id;
	}

	void Room::Nuke(uint32_t id) {
		std::erase_if(
				m_Objs,
				[=](const GameObject& obj) {
				return obj.Id == id;
				}
				);
	}

	void Room::Save() {
		std::string path;
		if (pfd::settings::available()) {
			pfd::save_file saveFileDiag = pfd::save_file("save location", pfd::path::home(),
				{ "Backrooms Room (.brr)", "*.brr",
				  "All Files", "*" });

			path = saveFileDiag.result();
		} else {
			path = "latest.brr";
		}

		FileWriter writer(path);

		writer.Str("brr");

		writer.U8(0);
		writer.U32(m_Objs.size());

		for (GameObject obj : m_Objs) {
			writer.U32(obj.Id);

			obj.ObjTransform.Write(writer);

			writer.Str(obj.Name);

			writer.U64(obj.Obj.index());

			std::visit([&](auto& val) {
					using T = std::decay_t<decltype(val)>;

					if constexpr (std::same_as<T, Object>) {
					writer.U8(val.Col.X);
					writer.U8(val.Col.Y);
					writer.U8(val.Col.Z);
					} else if constexpr (std::same_as<T, Mesh>) {
					writer.U8(val.Col.X);
					writer.U8(val.Col.Y);
					writer.U8(val.Col.Z);

					writer.Str(val.MeshId);
					} else if constexpr (std::same_as<T, Sound>) {
					writer.Str(val.SoundId);
					}
					}, obj.Obj);

			writer.U8(0);
		}

		writer.Flush();
	}

	void Room::Load() {
		std::string path;

		if (pfd::settings::available()) {
			pfd::open_file openFileDiag = pfd::open_file("save location", pfd::path::home(),
				{ "Backrooms Room (.brr)", "*.brr",
				  "All Files", "*" });

			path = openFileDiag.result().at(0);
		} else {
			path = "latest.brr";
		}

		FileReader reader("latest.brr");

		if (reader.Str() != "brr") {
			std::println("latest.brr is not a valid .brr");
			return;
		}

		if (reader.U8() != 0) {
			std::println("currently not supported .brr version");
			return;
		}

		uint32_t size = reader.U32();

		std::vector<GameObject> objs;
		objs.reserve(size);

		for (uint32_t i = 0; i < size; ++i) {
			uint32_t id = reader.U32();

			Transform transform = Transform::Read(reader);

			std::string name = reader.Str();

			uint64_t index = reader.U64();

			if (index == 0) {
				uint8_t r = reader.U8();
				uint8_t g = reader.U8();
				uint8_t b = reader.U8();

				std::string meshId = reader.Str();

				Mesh mesh({ .X = r, .Y = g, .Z = b }, meshId.c_str());

				GameObject gameObj(id, name.c_str(), transform, mesh);

				objs.emplace_back(gameObj);
			} else if (index == 1) {
				std::string soundId = reader.Str();

				Sound sound(soundId.c_str());

				GameObject gameObj(id, name.c_str(), transform, sound);

				objs.emplace_back(gameObj);
			} else if (index == 2) {
				uint8_t x = reader.U8();
				uint8_t y = reader.U8();
				uint8_t z = reader.U8();

				Object obj{ .Col = { .X = x, .Y = y, .Z = z }};

				GameObject gameObj(id, name.c_str(), transform, obj);

				objs.emplace_back(gameObj);
			}
		}

		m_Objs = objs;
	}

	uint32_t Room::GetSelectedId() {
		return m_Selected;
	}

	const std::vector<GameObject>& Room::Objects() {
		return m_Objs;
	}
}
