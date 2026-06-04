#pragma once

#include "raylib.h"
#include <memory>
#include <rlgl.h>
#include <unordered_map>

namespace BrrEdit {
	inline bool g_Wireframe = false;
	inline std::unordered_map<char*, Model> g_Models;
	// i really hate that  i have to use  UNIQUE_PTR here
	inline std::unordered_map<char*, std::unique_ptr<Music>> g_Sounds;
	inline bool g_CursorDisabled = false;
	inline Font g_TextFont = { 0 };

	constexpr float c_Speed       = 5.0f;
	constexpr float c_FasterSpeed = 12.0f;
	constexpr float c_Sensitivity = 0.003f;
}
