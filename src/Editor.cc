#include "Editor.hh"

#include "Globals.hh"

#include <algorithm>
#include <raylib.h>
#include <rlImGui.h>

#include <imgui.h>

namespace BrrEdit {
	Editor::Editor(int width, int height) : m_Width(width), m_Height(height) {
		SetTraceLogLevel(LOG_NONE);

		InitWindow(1920, 1080, "brredit");

		SetWindowState(FLAG_WINDOW_RESIZABLE);

		m_ViewportCam = { 0 };
		m_ViewportCam.position = { 10.0f, 10.0f, 10.0f };
		m_ViewportCam.target = { 0.0f, 0.0f, 0.0f };
		m_ViewportCam.up = { 0.0f, 1.0f, 0.0f };
		m_ViewportCam.fovy = 45.0f;
		m_ViewportCam.projection = CAMERA_PERSPECTIVE;

		SetTargetFPS(100);

		rlImGuiBeginInitImGui();

		ImGuiIO& io = ImGui::GetIO();
		io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
		io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;
		io.Fonts->Clear();
		io.Fonts->AddFontFromFileTTF("Roboto/Roboto-Medium.ttf", 16.0f);

		rlImGuiEndInitImGui();

		g_PbrShader = LoadShader("src/pbr.vs", "src/pbr.fs");

		SetupPbr();

		ApplyCatppuccinMocha();

		m_Viewport = LoadRenderTexture(1280 * 2, 720 * 2);

		SetTextureFilter(m_Viewport.texture, TEXTURE_FILTER_BILINEAR);

		while (!WindowShouldClose()) {
			BeginDrawing();
				ClearBackground(DARKGRAY);


				RenderImGui();
			EndDrawing();
		}
	}

	Editor::~Editor() {
		UnloadRenderTexture(m_Viewport);

		rlImGuiShutdown();

		CloseWindow();
	}

	void Editor::RenderImGui() {
		rlImGuiBegin();

		ImGui::DockSpaceOverViewport(0, ImGui::GetMainViewport());

		Hierarchy();
		Inspector();

		ImGui::Begin("Viewport");

		RenderRaylibWrapper();

		// Render scene into texture
		BeginTextureMode(m_Viewport);

		RenderRaylib();

		EndTextureMode();

		rlImGuiImageRenderTextureFit(&m_Viewport, true);

		ImGui::End();

		ImGui::Begin("Files");
		ImGui::End();

		rlImGuiEnd();
	}

	void Editor::Hierarchy() {
		ImGui::Begin("Hierarchy");

		if (ImGui::Button("Add")) {
			ImGui::OpenPopup("add");
		}

		if (ImGui::BeginPopup("add")) {
			if (ImGui::MenuItem("Object")) {
				m_Room.AddObject();
			}

			if (ImGui::MenuItem("Mesh")) {
				m_Room.AddMesh();
			}

			if (ImGui::MenuItem("Sound")) {
			}

			ImGui::EndPopup();
		}

		ImGui::End();
	}

	void Editor::Inspector() {
		ImGui::Begin("Inspector");

		if (m_Room.GetSelected() != nullptr) {
			GameObject* sel = m_Room.GetSelected();

			ImGui::InputText("Name", sel->Name, sizeof(sel->Name));

			auto ColorLabel = [](const char* id, const char* label, ImVec4 col) {
				ImGui::PushStyleColor(ImGuiCol_Button,        col);
				ImGui::PushStyleColor(ImGuiCol_ButtonHovered, col);
				ImGui::PushStyleColor(ImGuiCol_ButtonActive,  col);
				ImGui::PushStyleColor(ImGuiCol_Text,          { 1, 1, 1, 1 });
				ImGui::PushID(id);
				float h = ImGui::GetFrameHeight();
				ImGui::Button(label, { h, h });
				ImGui::PopID();
				ImGui::PopStyleColor(4);
			};

			float sliderWidth = (ImGui::GetContentRegionAvail().x / 3.0f)
				- ImGui::CalcTextSize("X").x
				- ImGui::GetStyle().FramePadding.x * 2.0f
				- ImGui::GetStyle().ItemSpacing.x
				- 10.0f;

			ImGui::PushItemWidth(sliderWidth);

			ImGui::Text("Position");

			ColorLabel("posxlabel", "X", { 0.8f, 0.1f, 0.1f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##posx", &sel->ObjTransform.Pos.X, 0.1f, -100.0f, 100.0f);

			ImGui::SameLine();

			ColorLabel("posylabel", "Y", { 0.1f, 0.7f, 0.1f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##posy", &sel->ObjTransform.Pos.Y, 0.1f, -100.0f, 100.0f);

			ImGui::SameLine();

			ColorLabel("poszlabel", "Z", { 0.1f, 0.1f, 0.8f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##posz", &sel->ObjTransform.Pos.Z, 0.1f, -100.0f, 100.0f);

			ImGui::Text("Scale");

			ColorLabel("scalexlabel", "X", { 0.8f, 0.1f, 0.1f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##scalex", &sel->ObjTransform.Scale.X, 0.1f, -100.0f, 100.0f);

			ImGui::SameLine();

			ColorLabel("scaleylabel", "Y", { 0.1f, 0.7f, 0.1f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##scaley", &sel->ObjTransform.Scale.Y, 0.1f, -100.0f, 100.0f);

			ImGui::SameLine();

			ColorLabel("scalezlabel", "Z", { 0.1f, 0.1f, 0.8f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##scalez", &sel->ObjTransform.Scale.Z, 0.1f, -100.0f, 100.0f);

			ImGui::Text("Rotation");

			ColorLabel("rotxlabel", "X", { 0.8f, 0.1f, 0.1f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##rotx", &sel->ObjTransform.Rot.X, 0.1f, -100.0f, 100.0f);

			ImGui::SameLine();

			ColorLabel("rotylabel", "Y", { 0.1f, 0.7f, 0.1f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##roty", &sel->ObjTransform.Rot.Y, 0.1f, -100.0f, 100.0f);

			ImGui::SameLine();

			ColorLabel("rotzlabel", "Z", { 0.1f, 0.1f, 0.8f, 1.0f }); ImGui::SameLine();
			ImGui::DragFloat("##rotz", &sel->ObjTransform.Rot.Z, 0.1f, -100.0f, 100.0f);

			ImGui::PopItemWidth();

			if (Object* obj = std::get_if<Object>(&sel->Obj)) {
				ColorEdit(obj->Col);
			} else if (Mesh* mesh = std::get_if<Mesh>(&sel->Obj)) {
				ColorEdit(mesh->Col);

				ImGui::InputText("Mesh ID", mesh->MeshId, sizeof(mesh->MeshId));
			}
		}

		ImGui::End();
	}

	void Editor::ColorEdit(Vec3b& col) {
		static float colBuf[3] = { 1.0f, 1.0f, 1.0f };

		ImGui::Text("Color");
		if (ImGui::ColorButton("Color", { colBuf[0], colBuf[1], colBuf[2], 1.0f })) {
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
				texH != m_Viewport.texture.height)
		{
			UnloadRenderTexture(m_Viewport);
			m_Viewport = LoadRenderTexture(texW * 2, texH * 2);
			SetTextureFilter(m_Viewport.texture, TEXTURE_FILTER_BILINEAR);
		}
	}

	void Editor::RenderRaylib() {
		ClearBackground(BLACK);

		if (IsKeyPressed(KEY_F)) {
			g_Wireframe = !g_Wireframe;
		}

		// account for 2x downscaling
		DrawFPS(10 * 2, 10 * 2);

		BeginMode3D(m_ViewportCam);

		DrawGrid(10, 1);

		float camPos[3] = { m_ViewportCam.position.x, m_ViewportCam.position.y, m_ViewportCam.position.z };
		SetShaderValue(g_PbrShader, g_PbrShader.locs[SHADER_LOC_VECTOR_VIEW], camPos, SHADER_UNIFORM_VEC3);

		m_Room.Render();

		EndMode3D();
	}
}
