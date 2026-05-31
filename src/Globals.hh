#pragma once

#include "raylib.h"
#include <unordered_map>
namespace BrrEdit {
	inline bool g_Wireframe = false;
	inline std::unordered_map<char*, Model> g_Models;
	inline Shader g_PbrShader = { 0 };
	inline int g_LightCountLoc = 0;

	inline void SetupPbr() {
		g_PbrShader.locs[SHADER_LOC_MAP_ALBEDO]    = GetShaderLocation(g_PbrShader, "albedoMap");
		g_PbrShader.locs[SHADER_LOC_MAP_NORMAL]    = GetShaderLocation(g_PbrShader, "normalMap");
		g_PbrShader.locs[SHADER_LOC_MAP_ROUGHNESS] = GetShaderLocation(g_PbrShader, "mraMap");
		g_PbrShader.locs[SHADER_LOC_VECTOR_VIEW]   = GetShaderLocation(g_PbrShader, "viewPos");

		// defaults
		float tiling[2] = { 1.0f, 1.0f };
		float offset[2] = { 0.0f, 0.0f };
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "tiling"),         tiling,        SHADER_UNIFORM_VEC2);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "offset"),         offset,        SHADER_UNIFORM_VEC2);

		float albedoColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "albedoColor"),    albedoColor,   SHADER_UNIFORM_VEC4);

		float emissiveColor[4] = { 0.0f, 0.0f, 0.0f, 1.0f };
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "emissiveColor"),  emissiveColor, SHADER_UNIFORM_VEC4);

		float ambientColor[3] = { 1.0f, 1.0f, 1.0f };
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "ambientColor"),   ambientColor,  SHADER_UNIFORM_VEC3);

		float ambient       = 0.5f;
		float normalValue   = 1.0f;
		float metallicValue = 0.0f;
		float roughnessValue= 0.5f;
		float aoValue       = 1.0f;
		float emissivePower = 0.0f;
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "ambient"),        &ambient,       SHADER_UNIFORM_FLOAT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "normalValue"),    &normalValue,   SHADER_UNIFORM_FLOAT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "metallicValue"),  &metallicValue, SHADER_UNIFORM_FLOAT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "roughnessValue"), &roughnessValue,SHADER_UNIFORM_FLOAT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "aoValue"),        &aoValue,       SHADER_UNIFORM_FLOAT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "emissivePower"),  &emissivePower, SHADER_UNIFORM_FLOAT);

		int useTexAlbedo = 1, useTexNormal = 1, useTexMRA = 1, useTexEmissive = 0;
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "useTexAlbedo"),   &useTexAlbedo,   SHADER_UNIFORM_INT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "useTexNormal"),   &useTexNormal,   SHADER_UNIFORM_INT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "useTexMRA"),      &useTexMRA,      SHADER_UNIFORM_INT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "useTexEmissive"), &useTexEmissive, SHADER_UNIFORM_INT);

		// one point light
		float lightPos[3] = { 5.0f, 10.0f, 5.0f };
		float lightColor[4] = { 1.0f, 1.0f, 1.0f, 1.0f };
		float lightIntensity = 100.0f;
		int lightEnabled = 1, lightType = 1;
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "lights[0].enabled"),   &lightEnabled,   SHADER_UNIFORM_INT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "lights[0].type"),      &lightType,      SHADER_UNIFORM_INT);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "lights[0].position"),  lightPos,        SHADER_UNIFORM_VEC3);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "lights[0].color"),     lightColor,      SHADER_UNIFORM_VEC4);
		SetShaderValue(g_PbrShader, GetShaderLocation(g_PbrShader, "lights[0].intensity"), &lightIntensity, SHADER_UNIFORM_FLOAT);

		int numLights = 1;
		g_LightCountLoc = GetShaderLocation(g_PbrShader, "numOfLights");
		SetShaderValue(g_PbrShader, g_LightCountLoc, &numLights, SHADER_UNIFORM_INT);
	}
}
