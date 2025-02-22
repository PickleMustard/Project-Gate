#include "attempt_to_discover_enemy_action.h"
#include "autonomous-agents/base_components/goap_agent.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/array.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/vector2i.hpp"
#include "level-generation/tilegrid.h"
#include "seeded_random_access.h"

using namespace godot;

AttemptToDiscoverEnemyAction::AttemptToDiscoverEnemyAction() :
		GoapAction("AttemptToDiscoverEnemyAction", 3.0f) {
	movement_remaining = Callable(this, "HasMovementRangeRemaining");
  found_enemy = Callable(this, "HasFoundEnemy");
	AddPrecondition("seen_enemy", false);
  AddPrecondition("has_enemy_within_range", false);
	AddPrecondition("movement_remaining", movement_remaining);
	AddEffect("targetEnemy", true);
}

AttemptToDiscoverEnemyAction::~AttemptToDiscoverEnemyAction() {
}

void AttemptToDiscoverEnemyAction::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("CheckProceduralPrecondition", "goap_agent", "world_data"), &AttemptToDiscoverEnemyAction::CheckProceduralPrecondition);
	godot::ClassDB::bind_method(godot::D_METHOD("Perform", "goap_agent"), &AttemptToDiscoverEnemyAction::Perform);
	godot::ClassDB::bind_method(godot::D_METHOD("HasMovementRangeRemaining", "distance"), &AttemptToDiscoverEnemyAction::HasMovementRangeRemaining);
  godot::ClassDB::bind_method(godot::D_METHOD("HasFoundEnemy"), &AttemptToDiscoverEnemyAction::HasFoundEnemy);
}

void AttemptToDiscoverEnemyAction::Reset() {
	UtilityFunctions::print("discover resetting");
	seen_enemy = false;
}

bool AttemptToDiscoverEnemyAction::IsDone(Node *goap_agent) {
	return seen_enemy || !HasMovementRangeRemaining(goap_agent->get_parent()->call("GetDistanceRemaining"));
}

bool AttemptToDiscoverEnemyAction::InProgress(Node *goap_agent) {
	bool is_moving = goap_agent->get_parent()->call("GetIsMoving");
	//UtilityFunctions::print("Is Moving Call: ", is_moving);
	return is_moving;
}
bool AttemptToDiscoverEnemyAction::RequiresInRange() {
	return false;
}

bool AttemptToDiscoverEnemyAction::HasMovementRangeRemaining(int distance) {
	return distance > 0;
}

bool AttemptToDiscoverEnemyAction::HasFoundEnemy() {
  return seen_enemy;
}

bool AttemptToDiscoverEnemyAction::GetInRange(Node *goap_agent, Dictionary world_data) {
  return true;
}

/* Attempts to discover enemies
 * If it hasn't seen an enemy ever, will move randomly to attempt to find one
 * If it has, will attempt to move towards the last known location of the closest enemy
 * If it gets to the location of the closest last known and doesn't see another, will go back to
 * random movement
 */
bool AttemptToDiscoverEnemyAction::CheckProceduralPrecondition(Node *goap_agent, Dictionary world_data) {
	//
	UtilityFunctions::print("Checking ", GetActionName(), " preconditions");
	if (world_data.has("seen_enemy")) {
		int seen_number = world_data["seen_enemy"];
		return seen_number < 1;
	}
	return false;
}

bool AttemptToDiscoverEnemyAction::Perform(Node *goap_agent) {
	UtilityFunctions::print("Performing action: ", GetActionName());
	Node *unit_controller = cast_to<Node>(goap_agent->call("GetMovementSystem"));
	UtilityFunctions::print("Got Controller");
	Array destinations = unit_controller->call("GetPotentialDestinations", goap_agent->get_parent());
	UtilityFunctions::print("Got Destinations: ", destinations.size());
	SeededRandomAccess *instance = SeededRandomAccess::GetInstance();
	int location = instance->GetWholeNumber(destinations.size() - 1);
	Dictionary meshDict = destinations[location];
	Node *tile = cast_to<Node>(meshDict["TileMesh"]);
	UtilityFunctions::print("Moving Character");
	unit_controller->call("MoveCharacter", goap_agent->get_parent(), tile->get_parent());
	UtilityFunctions::print("Moved Character");
	return true;
}
