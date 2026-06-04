#include "Editor.hh"

#include "GameObject.hh"
#include "Globals.hh"
#include "extras/IconsFontAwesome6.h"
#include <format>
#include <raymath.h>

#include <algorithm>
#include <cfloat>
#include <raylib.h>
#include <rlImGui.h>

#include <imgui.h>

namespace BrrEdit {
	Editor::Editor(int width, int height) : m_Width(width), m_Height(height) {
		// SetTraceLogLevel(LOG_NONE);

		InitWindow(1920, 1080, "brredit");

		SetWindowState(FLAG_WINDOW_RESIZABLE);

		InitAudioDevice();

		m_ViewportCam = {0};
		m_ViewportCam.position = {10.0f, 10.0f, 10.0f};
		m_ViewportCam.target = {0.0f, 0.0f, 0.0f};
		m_ViewportCam.up = {0.0f, 1.0f, 0.0f};
		m_ViewportCam.fovy = 45.0f;
		m_ViewportCam.projection = CAMERA_PERSPECTIVE;

		SetTargetFPS(100);

		rlImGuiBeginInitImGui();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.Fonts->Clear();
		auto roboto =
			io.Fonts->AddFontFromFileTTF("Roboto/Roboto-Medium.ttf", 16.0f);

		static const ImWchar iconRanges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};

		ImFontConfig cfg;
		cfg.PixelSnapH = true;

		io.Fonts->AddFontFromFileTTF("Roboto/fa.ttf", 16.0f, &cfg, iconRanges);

		io.FontDefault = roboto;

		rlImGuiEndInitImGui();

		g_TextFont = LoadFontEx("Roboto/Roboto-Medium.ttf", 128, 0, 0);
		SetTextureFilter(g_TextFont.texture, TEXTURE_FILTER_BILINEAR);

		ApplyCatppuccinMocha();

		m_Viewport = LoadRenderTexture(1280 * 2, 720 * 2);

		SetTextureFilter(m_Viewport.texture, TEXTURE_FILTER_BILINEAR);

		rlSetLineWidth(10);

		while (!WindowShouldClose()) {
			Update();

			BeginDrawing();
			ClearBackground(DARKGRAY);

			RenderImGui();
			EndDrawing();
		}
	}

	Editor::~Editor() {
		UnloadFont(g_TextFont);
		UnloadRenderTexture(m_Viewport);

		rlImGuiShutdown();

		CloseAudioDevice();

		CloseWindow();
	}

	void Editor::RenderImGui() {
		// Snapshot key state BEFORE rlImGuiBegin consumes IsKeyPressed events
		bool tabPressed = IsKeyPressed(KEY_TAB);
		bool fPressed = IsKeyPressed(KEY_F);

		rlImGuiBegin();

		if (ImGui::BeginMainMenuBar()) {
			if (ImGui::MenuItem("Save")) {
				m_Room.Save();
			}

			if (ImGui::MenuItem("Load")) {
				m_Room.Load();
			}

			ImGui::EndMainMenuBar();
		}

		ImGuiIO& io = ImGui::GetIO();

		if (g_CursorDisabled) {
			// When cursor is locked, prevent ImGui from overriding cursor and
			// push mouse far away to avoid spurious hover/click on UI widgets
			io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;
			io.AddMousePosEvent(-FLT_MAX, -FLT_MAX);
		} else {
			io.ConfigFlags &= ~ImGuiConfigFlags_NoMouseCursorChange;
		}

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		Hierarchy();
		Inspector();

		ImGui::Begin("Viewport");

		// mouse locking
		if (ImGui::IsWindowHovered() &&
			ImGui::IsMouseClicked(ImGuiMouseButton_Left)) {
			DisableCursor();
			g_CursorDisabled = true;
		}

		RenderRaylibWrapper();

		// Render scene into texture
		BeginTextureMode(m_Viewport);

		RenderRaylib(tabPressed, fPressed);

		EndTextureMode();

		rlImGuiImageRenderTextureFit(&m_Viewport, true);

		ImGui::End();

		rlImGuiEnd();
	}

	void Editor::Hierarchy() {
		ImGui::Begin("Hierarchy");

		ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
		if (ImGui::Button(ICON_FA_PLUS, {30, 30})) {
			ImGui::OpenPopup("add");
		}
		ImGui::PopFont();

		if (ImGui::BeginPopup("add")) {

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			if (ImGui::MenuItem(ICON_FA_CUBE)) {
				m_Room.AddObject();
			}
			ImGui::PopFont();

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			if (ImGui::MenuItem(ICON_FA_BORDER_ALL)) {
				m_Room.AddMesh();
			}
			ImGui::PopFont();

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			if (ImGui::MenuItem(ICON_FA_VOLUME_HIGH)) {
				m_Room.AddSound();
			}

			if (ImGui::MenuItem(ICON_FA_DISPLAY)) {
				m_Room.AddUI();
			}
			ImGui::PopFont();

			ImGui::EndPopup();
		}

		for (const GameObject& gameObj : m_Room.Objects()) {
			auto a = std::format("{}##{}", gameObj.Name, gameObj.Id);
			auto nuke = std::format("{}##{}_nuke", ICON_FA_TRASH, gameObj.Id);

			float btnWidth = ImGui::GetFrameHeight();
			float spacing = ImGui::GetStyle().ItemSpacing.x;

			float selectableWidth =
				ImGui::GetContentRegionAvail().x - btnWidth - spacing;

			ImGui::PushItemFlag(ImGuiItemFlags_NoTabStop, true);

			if (ImGui::Selectable(a.c_str(),
					gameObj.Id == m_Room.GetSelectedId(), 0,
					ImVec2(selectableWidth, 0))) {
				m_Room.Select(gameObj.Id);
			}

			ImGui::PopItemFlag();

			ImGui::SameLine();

			ImGui::PushStyleColor(ImGuiCol_ButtonHovered, {.8, 0, 0, 1});
			ImGui::PushStyleColor(ImGuiCol_ButtonActive, {.6, 0, 0, 1});
			ImGui::PushStyleColor(ImGuiCol_Button, {1, 0, 0, 1});

			ImGui::PushFont(ImGui::GetIO().Fonts->Fonts[1]);
			if (ImGui::SmallButton(nuke.c_str())) {
				m_Room.Nuke(gameObj.Id);
			}
			ImGui::PopFont();

			ImGui::PopStyleColor(3);
		}

		ImGui::End();
	}

	void Editor::Inspector() {
		ImGui::Begin("Inspector");

		if (m_Room.GetSelected() != nullptr) {
			GameObject* sel = m_Room.GetSelected();

			ImGui::InputText("Name", sel->Name, sizeof(sel->Name));

			auto ColorLabel = [](const char* id, const char* label,
								  ImVec4 col) {
				ImGui::PushStyleColor(ImGuiCol_Button, col);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive, col);
				ImGui::PushStyleColor(ImGuiCol_Text, {1, 1, 1, 1});
				ImGui::PushID(id);
				float h = ImGui::GetFrameHeight();
				ImGui::Button(label, {h, h});
				ImGui::PopID();
				ImGui::PopStyleColor(4);
			};

			float sliderWidth = (ImGui::GetContentRegionAvail().x / 3.0f) -
								ImGui::CalcTextSize("X").x -
								ImGui::GetStyle().FramePadding.x * 2.0f -
								ImGui::GetStyle().ItemSpacing.x - 10.0f;

			ImGui::PushItemWidth(sliderWidth);

			if (Text* text = std::get_if<Text>(&sel->Obj)) {
				ImGui::Text("Position");

				ColorLabel("txposxlabel", "X", {0.8f, 0.1f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat("##txposx", &text->Pos.X, 0.5f, 0.0f, 0.0f);
				ImGui::SameLine();

				ColorLabel("txposylabel", "Y", {0.1f, 0.7f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat("##txposy", &text->Pos.Y, 0.5f, 0.0f, 0.0f);

				ImGui::Text("Font Size");
				ImGui::DragFloat(
					"##fontsize", &text->FontSize, 0.5f, 0.0f, 0.0f);
			} else {
				ImGui::Text("Position");

				ColorLabel("posxlabel", "X", {0.8f, 0.1f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##posx", &sel->ObjTransform.Pos.X, 0.5f, 0.0f, 0.0f);

				ImGui::SameLine();

				ColorLabel("posylabel", "Y", {0.1f, 0.7f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##posy", &sel->ObjTransform.Pos.Y, 0.5f, 0.0f, 0.0f);

				ImGui::SameLine();

				ColorLabel("poszlabel", "Z", {0.1f, 0.1f, 0.8f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##posz", &sel->ObjTransform.Pos.Z, 0.5f, 0.0f, 0.0f);

				ImGui::Text("Scale");

				ColorLabel("scalexlabel", "X", {0.8f, 0.1f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##scalex", &sel->ObjTransform.Scale.X, 0.5f, 0.0f, 0.0f);

				ImGui::SameLine();

				ColorLabel("scaleylabel", "Y", {0.1f, 0.7f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##scaley", &sel->ObjTransform.Scale.Y, 0.5f, 0.0f, 0.0f);

				ImGui::SameLine();

				ColorLabel("scalezlabel", "Z", {0.1f, 0.1f, 0.8f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##scalez", &sel->ObjTransform.Scale.Z, 0.5f, 0.0f, 0.0f);

				ImGui::Text("Rotation");

				ColorLabel("rotxlabel", "X", {0.8f, 0.1f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##rotx", &sel->ObjTransform.Rot.X, 0.5f, 0.0f, 0.0f);

				ImGui::SameLine();

				ColorLabel("rotylabel", "Y", {0.1f, 0.7f, 0.1f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##roty", &sel->ObjTransform.Rot.Y, 0.5f, 0.0f, 0.0f);

				ImGui::SameLine();

				ColorLabel("rotzlabel", "Z", {0.1f, 0.1f, 0.8f, 1.0f});
				ImGui::SameLine();
				ImGui::DragFloat(
					"##rotz", &sel->ObjTransform.Rot.Z, 0.5f, 0.0f, 0.0f);
			}

			ImGui::PopItemWidth();

			if (Object* obj = std::get_if<Object>(&sel->Obj)) {
				ColorEdit(obj->Col);
			} else if (Mesh* mesh = std::get_if<Mesh>(&sel->Obj)) {
				ColorEdit(mesh->Col);

				ImGui::InputText("Mesh ID", mesh->MeshId, sizeof(mesh->MeshId));
			} else if (Sound* sound = std::get_if<Sound>(&sel->Obj)) {
				ImGui::InputText(
					"Sound ID", sound->SoundId, sizeof(sound->SoundId));
			} else if (Text* text = std::get_if<Text>(&sel->Obj)) {
				ColorEdit(text->Col);

				ImGui::InputText("Text", text->Txt, sizeof(text->Txt));
			}
		}

		ImGui::End();
	}

	void Editor::ColorEdit(Vec3b& col) {
		static float colBuf[3] = {1.0f, 1.0f, 1.0f};

		ImGui::Text("Color");
		if (ImGui::ColorButton(
				"Color", {colBuf[0], colBuf[1], colBuf[2], 1.0f})) {
			ImGui::OpenPopup("color");
		}

		if (ImGui::BeginPopup("color")) {
			if (ImGui::ColorPicker3("Color", colBuf)) {
				col = Vec3b::FromFloat3(colBuf);
			}

			ImGui::EndPopup();
		}
	}

	void Editor::RenderRaylibWrapper() {
		ImVec2 size = ImGui::GetContentRegionAvail();

		int texW = std::max(1, (int)size.x);
		int texH = std::max(1, (int)size.y);

		if (texW != m_Viewport.texture.width ||
			texH != m_Viewport.texture.height) {
			UnloadRenderTexture(m_Viewport);
			m_Viewport = LoadRenderTexture(texW * 2, texH * 2);
			SetTextureFilter(m_Viewport.texture, TEXTURE_FILTER_BILINEAR);
		}
	}

	void Editor::RenderRaylib(bool tabPressed, bool fPressed) {
		ClearBackground(BLACK);

		if (fPressed) {
			g_Wireframe = !g_Wireframe;
		}

		Vector2 mouse = g_CursorDisabled ? GetMouseDelta() : Vector2{0, 0};

		Vector3 dir =
			Vector3Subtract(m_ViewportCam.target, m_ViewportCam.position);
		Vector3 right =
			Vector3Normalize(Vector3CrossProduct(dir, m_ViewportCam.up));

		dir =
			Vector3RotateByAxisAngle(dir, {0, 1, 0}, -mouse.x * c_Sensitivity);

		Vector3 pitched =
			Vector3RotateByAxisAngle(dir, right, -mouse.y * c_Sensitivity);
		if (fabsf(Vector3DotProduct(Vector3Normalize(pitched), {0, 1, 0})) <
			0.99f)
			dir = pitched;

		m_ViewportCam.target = Vector3Add(m_ViewportCam.position, dir);

		Vector3 forward = Vector3Normalize(dir);

		auto move = [&](Vector3 delta) {
			m_ViewportCam.position = Vector3Add(m_ViewportCam.position, delta);
			m_ViewportCam.target = Vector3Add(m_ViewportCam.target, delta);
		};

		if (tabPressed) {
			EnableCursor();
			g_CursorDisabled = false;
		}

		float dt = GetFrameTime();

		if (g_CursorDisabled) {
			const float speed =
				IsKeyDown(KEY_CAPS_LOCK) ? c_FasterSpeed : c_Speed;

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

			if (IsKeyDown(KEY_ZERO)) {
				m_ViewportCam.position = {10.0f, 10.0f, 10.0f};
				m_ViewportCam.target = {0.0f, 0.0f, 0.0f};
			}
		}

		// account for 2x downscaling
		DrawFPS(10 * 2, 10 * 2);

		BeginMode3D(m_ViewportCam);

		this->DrawGrid(100, 1);

		m_Room.Render();

		EndMode3D();

		m_Room.RenderUI();
	}

	void Editor::DrawGrid(int slices, float spacing) {
		int halfSlices = slices / 2;

		rlBegin(RL_LINES);
		for (int i = -halfSlices; i <= halfSlices; i++) {
			rlColor4ub(GRAY.r, GRAY.g, GRAY.b, 138);

			rlVertex3f((float)i * spacing, 0.0f, (float)-halfSlices * spacing);
			rlVertex3f((float)i * spacing, 0.0f, (float)halfSlices * spacing);

			rlVertex3f((float)-halfSlices * spacing, 0.0f, (float)i * spacing);
			rlVertex3f((float)halfSlices * spacing, 0.0f, (float)i * spacing);
		}
		rlEnd();
	}

	void Editor::Update() {
		m_Room.Update(m_ViewportCam);
	}
}  // namespace BrrEdit
