#pragma once

#include "raylib.h"
#include <memory>
#include <unordered_map>

namespace BrrEdit {
	inline bool g_Wireframe = false;
	inline std::unordered_map<char*, Model> g_Models;
	inline std::unordered_map<char*, std::unique_ptr<Music>> g_Sounds;
	inline Font g_TextFont = { 0 };
}
