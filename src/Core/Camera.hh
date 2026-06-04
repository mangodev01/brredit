#pragma once

#include <raylib.h>

namespace BrrEdit {
	struct Camera {
		Camera3D Cam = { 0 };
		float Speed = 5.0f;
		float FastSpeed = 12.0f;
		float Sensitivity = 0.003f;

		void Update(float dt, Vector2 mouseDelta, bool fast);
		void Reset();
	};
}
