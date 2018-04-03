#pragma once

#include "comp_base.h"
#include "comp_camera.h"

class CTexture;
class CRenderToTexture;

class TCompLight : public TCompCamera {

	std::string type; // replace this

	VEC4			color = VEC4(1, 1, 1, 1);
	float			intensity = 1.0f;
	const CTexture* projector = nullptr;

	// Shadows params
	bool              shadows_enabled = false;    // Dynamic
	bool              casts_shadows = false;      // Static
	int               shadows_resolution = 256;
	float             shadows_step = 1.f;
	CRenderToTexture* shadows_rt = nullptr;

	void onCreate(const TMsgEntityCreated& msg);
	void onDestroy(const TMsgEntityDestroyed& msg);

	DECL_SIBLING_ACCESS();
public:

	/* spotlight parameters */
	float range;
	float angle;

	void debugInMenu();
	void renderDebug();
	void load(const json& j, TEntityParseContext& ctx);
	void update(float dt);

	void activate();
	void generateShadowMap();
	const std::string getType() { return type; } // refactor

	static void registerMsgs();
};