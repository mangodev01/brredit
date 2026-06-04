#include "Camera.hh"
#include <raymath.h>

namespace BrrEdit {
	void Camera::Update(float dt, Vector2 mouseDelta, bool fast) {
		Vector3 dir = Vector3Subtract(Cam.target, Cam.position);
		Vector3 right = Vector3Normalize(Vector3CrossProduct(dir, Cam.up));

		dir = Vector3RotateByAxisAngle(
			dir, {0, 1, 0}, -mouseDelta.x * Sensitivity);

		Vector3 pitched =
			Vector3RotateByAxisAngle(dir, right, -mouseDelta.y * Sensitivity);
		if (fabsf(Vector3DotProduct(Vector3Normalize(pitched), {0, 1, 0})) <
			0.99f)
			dir = pitched;

		Cam.target = Vector3Add(Cam.position, dir);
		Vector3 forward = Vector3Normalize(dir);

		float speed = fast ? FastSpeed : Speed;

		auto move = [&](Vector3 delta) {
			Cam.position = Vector3Add(Cam.position, delta);
			Cam.target = Vector3Add(Cam.target, delta);
		};

		if (IsKeyDown(KEY_W))
			move(Vector3Scale(forward, speed * dt));
		if (IsKeyDown(KEY_S))
			move(Vector3Scale(forward, -speed * dt));
		if (IsKeyDown(KEY_A))
			move(Vector3Scale(right, -speed * dt));
		if (IsKeyDown(KEY_D))
			move(Vector3Scale(right, speed * dt));
		if (IsKeyDown(KEY_SPACE))
			move({0, speed * dt, 0});
		if (IsKeyDown(KEY_LEFT_SHIFT))
			move({0, -speed * dt, 0});
	}

	void Camera::Reset() {
		Cam.position = {10.0f, 10.0f, 10.0f};
		Cam.target = {0.0f, 0.0f, 0.0f};
		Cam.up = {0.0f, 1.0f, 0.0f};
		Cam.fovy = 45.0f;
		Cam.projection = CAMERA_PERSPECTIVE;
	}
}
