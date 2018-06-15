#include "mcv_platform.h"
#include "comp_collider.h"
#include "comp_rigidbody.h"
#include "components/comp_transform.h"
#include "physics/physics_collider.h"
#include "components/comp_name.h"

DECL_OBJ_MANAGER("collider", TCompCollider);

TCompCollider::~TCompCollider(){

	// In case it's a controller, delegate it's destruction to the rigidbody.
	if (config->actor != nullptr){

		if (!config->is_controller) {
			config->actor->release();
			config->actor = nullptr;
		}
	}
}

void TCompCollider::debugInMenu() {

	config->debugInMenu();
  ImGui::Text("Collider shape: %s", shapeName.c_str());
  ImGui::Text("Group: %s", groupName.c_str());
  ImGui::Text("Mask: %s", maskName.c_str());
  physx::PxVec3 pos = config->actor->getGlobalPose().p;
  ImGui::Text("Collider position: %f %f %f", (float)pos.x, (float)pos.y, (float)pos.z);
}

void TCompCollider::load(const json& j, TEntityParseContext& ctx) {

	// Factory pattern inside the json loader.
	shapeName = j["shape"].get<std::string>();
	if (strcmp("box", shapeName.c_str()) == 0)
	{
		config = new CPhysicsBox();
	}
	else if (strcmp("sphere", shapeName.c_str()) == 0)
	{
		config = new CPhysicsSphere();
	}
	else if (strcmp("plane", shapeName.c_str()) == 0)
	{
		config = new CPhysicsPlane();
	}
	else if (strcmp("capsule", shapeName.c_str()) == 0)
	{
		config = new CPhysicsCapsule();
	}
	else if (strcmp("convex", shapeName.c_str()) == 0)
	{
		config = new CPhysicsConvex();
	}
	else if (strcmp("mesh", shapeName.c_str()) == 0)
	{
		config = new CPhysicsTriangleMesh();
	}

  groupName = j.value("group", "all");
  maskName = j.value("mask", "all");

	config->group = getFilterByName(groupName);
	config->mask = getFilterByName(maskName);
	config->is_trigger = j.value("is_trigger", false);

	if (j.count("center"))
	{
		VEC3 center = loadVEC3(j["center"]);
		config->center = physx::PxVec3(center.x, center.y, center.z);
	}

	config->load(j, ctx);
}

void TCompCollider::registerMsgs() {

	DECL_MSG(TCompCollider, TMsgEntityCreated, onCreate);
	DECL_MSG(TCompCollider, TMsgEntitiesGroupCreated, onGroupCreated);
	DECL_MSG(TCompCollider, TMsgTriggerEnter, onTriggerEnter);
	DECL_MSG(TCompCollider, TMsgTriggerExit, onTriggerExit);
	DECL_MSG(TCompCollider, TMsgEntityDestroyed, onDestroy);
}

void TCompCollider::createCollider()
{
  TCompTransform * compTransform = get<TCompTransform>();

  TCompName * name = get<TCompName>();
  // Create the shape, the actor and set the user data
  physx::PxShape * shape = config->createShape();
  config->createStatic(shape, compTransform);
  config->actor->userData = CHandle(this).asVoidPtr();
}

void TCompCollider::onCreate(const TMsgEntityCreated& msg) {

	TCompRigidbody * c_rigidbody = get<TCompRigidbody>();
	
	// Let the rigidbody handle the creation if it exists..
	if (c_rigidbody == nullptr)
	{
    createCollider();
	}
}

void TCompCollider::onGroupCreated(const TMsgEntitiesGroupCreated& msg) {
  TCompRigidbody * c_rigidbody = get<TCompRigidbody>();

  // Let the rigidbody handle the creation if it exists..
  if (c_rigidbody == nullptr && config->actor == nullptr)
  {
    createCollider();
  }
}

void TCompCollider::onDestroy(const TMsgEntityDestroyed & msg)
{
	//delete this;
}

void TCompCollider::onTriggerEnter(const TMsgTriggerEnter& msg) {

	std::map<uint32_t, TCompTransform*>::iterator it = handles.begin();
	uint32_t ext_index = msg.h_other_entity.getExternalIndex();
	if (handles.find(ext_index) == handles.end()){
		CEntity * c_other = msg.h_other_entity;
		TCompCollider * c_collider = c_other->get<TCompCollider>();
		TCompTransform * c_transform = c_other->get<TCompTransform>();
		assert(c_transform);

		handles[ext_index] = c_transform;

		if (c_collider->config->group & FilterGroup::Player)
		{
			player_inside = true;
		}
	}

	// Get all entities with given tag to test with
	// Send a message to all of them
}

void TCompCollider::onTriggerExit(const TMsgTriggerExit& msg) {

	auto it = handles.find(msg.h_other_entity.getExternalIndex());
	if (it != handles.end())
	{
		handles.erase(it);
		CEntity * c_other = msg.h_other_entity;
		TCompCollider * c_collider = c_other->get<TCompCollider>();

		if (c_collider->config->group & FilterGroup::Player)
		{
			player_inside = false;
		}
	}
}

void TCompCollider::update(float dt) {


}

bool TCompCollider::collisionDistance(const VEC3 & org, const VEC3 & dir, float maxDistance) {

	physx::PxRaycastHit hit;
	if (EnginePhysics.Raycast(org, dir, maxDistance, hit, physx::PxQueryFlag::eSTATIC)) {
		return hit.distance < maxDistance ? true : false;
	}

	return false;
}

void TCompCollider::setGlobalPose(VEC3 newPos, VEC4 newRotation, bool autowake)
{
  physx::PxTransform transform(physx::PxVec3(newPos.x, newPos.y, newPos.z), physx::PxQuat(newRotation.x, newRotation.y, newRotation.z, newRotation.w));
  config->actor->setGlobalPose(transform, autowake);
}
