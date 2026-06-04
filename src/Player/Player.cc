#include "Player.hh"

#include "Core/Globals.hh"
#include <raylib.h>
#include <raymath.h>

namespace BrrEdit {
	Player::Player(int argc, char** argv) {
		SetConfigFlags(FLAG_WINDOW_HIGHDPI);
		InitWindow(1920, 1080, "brredit - player");
		DisableCursor();
		SetTargetFPS(100);

		InitAudioDevice();

		m_Camera.Reset();

		g_TextFont = LoadFontEx("Roboto/Roboto-Medium.ttf", 128, 0, 0);
		SetTextureFilter(g_TextFont.texture, TEXTURE_FILTER_BILINEAR);

		std::string path = "latest.brr";
		if (argc > 1) {
			path = argv[1];
		}

		m_Room.LoadFrom(path);

		while (!WindowShouldClose()) {
			Update();
			Render();
		}
	}

	Player::~Player() {
		UnloadFont(g_TextFont);
		CloseAudioDevice();
		CloseWindow();
	}

	void Player::Update() {
		bool cursorLocked = IsCursorHidden();

		if (IsKeyPressed(KEY_TAB)) {
			if (cursorLocked) {
				EnableCursor();
			} else {
				DisableCursor();
			}
		}

		Vector2 mouse = cursorLocked ? GetMouseDelta() : Vector2{0, 0};
		m_Camera.Update(GetFrameTime(), mouse, IsKeyDown(KEY_CAPS_LOCK));

		if (IsKeyPressed(KEY_ZERO)) {
			m_Camera.Reset();
		}

		m_Room.Update(m_Camera.Cam);
	}

	void Player::Render() {
		BeginDrawing();
		ClearBackground(BLACK);

		m_Room.RenderUI();

		BeginMode3D(m_Camera.Cam);

		m_Room.Render();

		EndMode3D();

		DrawFPS(10, 10);

		EndDrawing();
	}

}
