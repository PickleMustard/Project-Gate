#include "attempt_to_discover_enemy_action.h"
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
	AddPrecondition("seen_enemy", false);
	AddPrecondition("movement_remaining", movement_remaining);
	AddEffect("attempting_to_find_enemy", true);
}

AttemptToDiscoverEnemyAction::~AttemptToDiscoverEnemyAction() {
}

void AttemptToDiscoverEnemyAction::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("CheckProceduralPrecondition", "goap_agent", "world_data"), &AttemptToDiscoverEnemyAction::CheckProceduralPrecondition);
	godot::ClassDB::bind_method(godot::D_METHOD("Perform", "goap_agent"), &AttemptToDiscoverEnemyAction::Perform);
	godot::ClassDB::bind_method(godot::D_METHOD("HasMovementRangeRemaining", "distance"), &AttemptToDiscoverEnemyAction::HasMovementRangeRemaining);
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
	UtilityFunctions::print("Is Moving Call: ", is_moving);
	return is_moving;
}
bool AttemptToDiscoverEnemyAction::RequiresInRange() {
	return false;
}

bool AttemptToDiscoverEnemyAction::HasMovementRangeRemaining(int distance) {
	return distance > 0;
}

bool AttemptToDiscoverEnemyAction::CheckProceduralPrecondition(Node *goap_agent, Dictionary world_data) {
	//Has to have seen an enemy recently and it is in RequiresInRange
	UtilityFunctions::print("Checking ", GetActionName(), " preconditions");
	return true;
}

bool AttemptToDiscoverEnemyAction::Perform(Node *goap_agent) {
	UtilityFunctions::print("Performing action: ", GetActionName());
	Node *unit_controller = cast_to<Node>(goap_agent->call("GetUnitController"));
	Array destinations = unit_controller->call("GetPotentialDestinations");
	SeededRandomAccess *instance = SeededRandomAccess::GetInstance();
	int location = instance->GetWholeNumber(destinations.size() - 1);
  Dictionary meshDict = destinations[location];
	Node *tile = cast_to<Node>(meshDict["TileMesh"]);
	unit_controller->call("MoveCharacter", tile->get_parent());
	return true;
}
