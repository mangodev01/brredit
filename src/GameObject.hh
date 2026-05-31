#pragma once

#include <cstring>
#include <raylib.h>
#include <cstdint>
#include <variant>
#include <vector>

namespace BrrEdit {
	struct Vec3b {
		uint8_t X;
		uint8_t Y;
		uint8_t Z;

		static Vec3b Zero() { return { 0, 0, 0 }; }
		static Vec3b Max() { return { 255, 255, 255 }; }

		static Vec3b FromFloat3(float col[3]) {
			return {
				(uint8_t)(col[0] * 255.0f),
				(uint8_t)(col[1] * 255.0f),
				(uint8_t)(col[2] * 255.0f)
			};
		}
	};

	struct Vec3f {
		float X;
		float Y;
		float Z;

		static Vec3f Zero() { return { 0, 0, 0 }; }
		static Vec3f One() { return { 1, 1, 1 }; }
	};

	struct Mesh {
		Vec3b Col;
		char MeshId[128];

		Mesh(Vec3b col, const char* meshId)
			: Col(col)
		{
			std::strncpy(MeshId, meshId, sizeof(MeshId) - 1);
			MeshId[sizeof(MeshId) - 1] = '\0';
		}
	};

	struct Sound {
		char* SoundId;
	};

	struct Object {
		Vec3b Col;
	};

	struct Transform {
		Vec3f Pos;

		Vec3f Scale;

		Vec3f Rot;

		static Transform Default() {
			return Transform { .Pos = Vec3f::Zero(), .Scale = Vec3f::One(), .Rot = Vec3f::Zero() };
		}
	};


	struct GameObject {
		uint32_t Id;
		char Name[128];
		Transform ObjTransform;
		std::variant<Mesh, Sound, Object> Obj;

		GameObject(uint32_t id, const char* name, Transform t, std::variant<Mesh, Sound, Object> obj)
			: Id(id), ObjTransform(t), Obj(std::move(obj))
		{
			std::strncpy(Name, name, sizeof(Name) - 1);
			Name[sizeof(Name) - 1] = '\0';
		}
	};

	class Room {
		public:
			Room();
			~Room();

			void Render();

			void AddObject();
			void AddMesh();

			GameObject* GetSelected();

		private:
			std::vector<GameObject> m_Objs;
			uint32_t m_Selected;
	};
}
