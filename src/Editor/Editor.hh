#pragma once

#include <raylib.h>
#include <imgui.h>

#include "Core/GameObject.hh"
#include "Core/Camera.hh"

namespace BrrEdit {
	class Editor {
		public:
			Editor(int width, int height);
			~Editor();

		private:
			void Update();

			void DrawGrid(int slices, float spacing);

			void RenderImGui();
			void RenderRaylib(bool tabPressed, bool fPressed);
			void RenderRaylibWrapper();

			void Hierarchy();
			void Inspector();

			inline void ApplyCatppuccinMocha() {
				ImGuiStyle& style = ImGui::GetStyle();

				style.WindowRounding   = 10.0f;
				style.FrameRounding    = 5.0f;
				style.PopupRounding    = 12.0f;
				style.ScrollbarRounding = 10.0f;
				style.GrabRounding     = 6.0f;
				style.TabRounding      = 12.0f;
				style.ChildRounding    = 12.0f;

				style.WindowPadding  = { 10.0f, 10.0f };
				style.FramePadding   = { 8.0f, 8.0f };
				style.ItemSpacing    = { 10.0f, 10.0f };
				style.IndentSpacing  = 20.0f;
				style.ScrollbarSize  = 16.0f;
				style.Alpha          = 1.0f;

				ImVec4* c = style.Colors;
				c[ImGuiCol_Text]                  = { 0.804f, 0.839f, 0.957f, 1.0f };
				c[ImGuiCol_TextDisabled]          = { 0.729f, 0.761f, 0.871f, 1.0f };
				c[ImGuiCol_WindowBg]              = { 0.118f, 0.118f, 0.180f, 1.0f };
				c[ImGuiCol_ChildBg]               = { 0.094f, 0.094f, 0.145f, 1.0f };
				c[ImGuiCol_PopupBg]               = { 0.094f, 0.094f, 0.145f, 1.0f };
				c[ImGuiCol_Border]                = { 0.067f, 0.067f, 0.106f, 1.0f };
				c[ImGuiCol_BorderShadow]          = { 0.0f,   0.0f,   0.0f,   0.0f };
				c[ImGuiCol_FrameBg]               = { 0.271f, 0.278f, 0.353f, 1.0f };
				c[ImGuiCol_FrameBgHovered]        = { 0.345f, 0.357f, 0.439f, 1.0f };
				c[ImGuiCol_FrameBgActive]         = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_TitleBg]               = { 0.067f, 0.067f, 0.106f, 1.0f };
				c[ImGuiCol_TitleBgActive]         = { 0.094f, 0.094f, 0.145f, 1.0f };
				c[ImGuiCol_TitleBgCollapsed]      = { 0.0f,   0.0f,   0.0f,   0.0f };
				c[ImGuiCol_MenuBarBg]             = { 0.067f, 0.067f, 0.106f, 1.0f };
				c[ImGuiCol_ScrollbarBg]           = { 0.190f, 0.190f, 0.200f, 1.0f };
				c[ImGuiCol_ScrollbarGrab]         = { 0.310f, 0.310f, 0.350f, 1.0f };
				c[ImGuiCol_ScrollbarGrabHovered]  = { 0.610f, 0.610f, 0.610f, 1.0f };
				c[ImGuiCol_ScrollbarGrabActive]   = { 0.498f, 0.518f, 0.612f, 1.0f };
				c[ImGuiCol_CheckMark]             = { 0.260f, 0.590f, 0.980f, 1.0f };
				c[ImGuiCol_SliderGrab]            = { 0.260f, 0.590f, 0.980f, 1.0f };
				c[ImGuiCol_SliderGrabActive]      = { 0.0f,   0.224f, 1.0f,   0.902f };
				c[ImGuiCol_Button]                = { 0.271f, 0.278f, 0.353f, 1.0f };
				c[ImGuiCol_ButtonHovered]         = { 0.345f, 0.357f, 0.439f, 1.0f };
				c[ImGuiCol_ButtonActive]          = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_Header]                = { 0.271f, 0.278f, 0.353f, 1.0f };
				c[ImGuiCol_HeaderHovered]         = { 0.345f, 0.357f, 0.439f, 1.0f };
				c[ImGuiCol_HeaderActive]          = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_Separator]             = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_SeparatorHovered]      = { 0.345f, 0.357f, 0.439f, 1.0f };
				c[ImGuiCol_SeparatorActive]       = { 0.498f, 0.518f, 0.612f, 1.0f };
				c[ImGuiCol_ResizeGrip]            = { 0.271f, 0.278f, 0.353f, 1.0f };
				c[ImGuiCol_ResizeGripHovered]     = { 0.345f, 0.357f, 0.439f, 1.0f };
				c[ImGuiCol_ResizeGripActive]      = { 0.260f, 0.590f, 0.980f, 1.0f };
				c[ImGuiCol_Tab]                   = { 0.118f, 0.118f, 0.180f, 1.0f };
				c[ImGuiCol_TabHovered]            = { 0.345f, 0.357f, 0.439f, 1.0f };
				c[ImGuiCol_TabActive]             = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_TabUnfocused]          = { 0.118f, 0.118f, 0.180f, 1.0f };
				c[ImGuiCol_TabUnfocusedActive]    = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_DockingPreview]        = { 0.260f, 0.590f, 0.980f, 0.350f };
				c[ImGuiCol_DockingEmptyBg]        = { 0.200f, 0.300f, 0.500f, 0.700f };
				c[ImGuiCol_PlotLines]             = { 0.498f, 0.518f, 0.612f, 1.0f };
				c[ImGuiCol_PlotLinesHovered]      = { 1.0f,   0.430f, 0.350f, 1.0f };
				c[ImGuiCol_PlotHistogram]         = { 0.650f, 0.890f, 0.631f, 1.0f };
				c[ImGuiCol_PlotHistogramHovered]  = { 0.900f, 0.700f, 0.0f,   1.0f };
				c[ImGuiCol_TableHeaderBg]         = { 0.067f, 0.067f, 0.106f, 1.0f };
				c[ImGuiCol_TableBorderStrong]     = { 0.192f, 0.196f, 0.267f, 1.0f };
				c[ImGuiCol_TableBorderLight]      = { 0.118f, 0.118f, 0.180f, 1.0f };
				c[ImGuiCol_TableRowBg]            = { 0.0f,   0.0f,   0.0f,   0.0f };
				c[ImGuiCol_TableRowBgAlt]         = { 1.0f,   1.0f,   1.0f,   0.060f };
				c[ImGuiCol_TextSelectedBg]        = { 0.260f, 0.590f, 0.980f, 0.350f };
				c[ImGuiCol_DragDropTarget]        = { 1.0f,   0.600f, 0.0f,   1.0f };
				c[ImGuiCol_NavHighlight]          = { 0.260f, 0.590f, 0.980f, 1.0f };
				c[ImGuiCol_NavWindowingHighlight] = { 1.0f,   1.0f,   1.0f,   0.700f };
				c[ImGuiCol_NavWindowingDimBg]     = { 0.800f, 0.800f, 0.800f, 0.200f };
				c[ImGuiCol_ModalWindowDimBg]      = { 0.800f, 0.800f, 0.800f, 0.350f };
			}

			void ColorEdit(Vec3b& col);

		private:
			int m_Width;
			int m_Height;
			Camera m_Camera;
			RenderTexture2D m_Viewport;
			Room m_Room;
			bool m_Wireframe = true;
			bool m_CursorDisabled = false;
	};
}
