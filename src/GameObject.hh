#pragma once

#include "File.hh"
#include <cstring>
#include <raylib.h>
#include <raymath.h>
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

		static Vec3f FromVector3(Vector3 v) {
			return { v.x, v.y, v.z };
		}

		Vec3f operator+(const Vec3f& rhs) const {
			return { X + rhs.X, Y + rhs.Y, Z + rhs.Z };
		}

		Vec3f operator-(const Vec3f& rhs) const {
			return { X - rhs.X, Y - rhs.Y, Z - rhs.Z };
		}

		Vec3f operator*(float rhs) const {
			return { X * rhs, Y * rhs, Z * rhs };
		}

		Vec3f operator/(float rhs) const {
			return { X / rhs, Y / rhs, Z / rhs };
		}

		Vec3f& operator+=(const Vec3f& rhs) {
			X += rhs.X;
			Y += rhs.Y;
			Z += rhs.Z;
			return *this;
		}

		Vec3f& operator-=(const Vec3f& rhs) {
			X -= rhs.X;
			Y -= rhs.Y;
			Z -= rhs.Z;
			return *this;
		}

		Vec3f& operator*=(float rhs) {
			X *= rhs;
			Y *= rhs;
			Z *= rhs;
			return *this;
		}

		Vec3f& operator/=(float rhs) {
			X /= rhs;
			Y /= rhs;
			Z /= rhs;
			return *this;
		}
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
		char SoundId[128];

		Sound(const char* soundId)
		{
			std::strncpy(SoundId, soundId, sizeof(SoundId) - 1);
			SoundId[sizeof(SoundId) - 1] = '\0';
		}
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

		inline void Write(FileWriter& writer) {
			writer.F32(Pos.X);
			writer.F32(Pos.Y);
			writer.F32(Pos.Z);

			writer.F32(Scale.X);
			writer.F32(Scale.Y);
			writer.F32(Scale.Z);

			writer.F32(Rot.X);
			writer.F32(Rot.Y);
			writer.F32(Rot.Z);
		}

		inline static Transform Read(FileReader& reader) {
			return { .Pos = { reader.F32(), reader.F32(), reader.F32() }, .Scale = { reader.F32(), reader.F32(), reader.F32() }, .Rot = { reader.F32(), reader.F32(), reader.F32() } };
		}
	};

	inline bool PointInOBB(const Vec3f& point, const Transform& box)
	{
		Vec3f local = point - box.Pos;

		// Inverse rotation
		Matrix invRot =
			MatrixRotateXYZ({
					-box.Rot.X,
					-box.Rot.Y,
					-box.Rot.Z
					});

		local = Vec3f::FromVector3(Vector3Transform(std::bit_cast<Vector3>(local), invRot));

		Vec3f half = box.Scale * 0.5f;

		return
			fabsf(local.X) <= half.X &&
			fabsf(local.Y) <= half.Y &&
			fabsf(local.Z) <= half.Z;
	}


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

			void Update(Camera3D cam);

			void Render();

			void AddObject();
			void AddMesh();
			void AddSound();

			void Save();
			void Load();

			GameObject* GetSelected();
			uint32_t GetSelectedId();

			void Select(uint32_t id);
			void Nuke(uint32_t id);

			const std::vector<GameObject>& Objects();

		private:
			std::vector<GameObject> m_Objs;
			uint32_t m_Selected;
	};
}
