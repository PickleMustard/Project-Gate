#include "attack_enemy_action.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level-generation/tilegrid.h"

using namespace godot;

AttackEnemyAction::AttackEnemyAction() : GoapAction("AttackEnemyAction", 3.0f) {
	AddPrecondition("seen_enemy", true);
	AddPrecondition("has_enemy_within_range", true);
}

AttackEnemyAction::~AttackEnemyAction() {
}

void AttackEnemyAction::_bind_methods() {
}

void AttackEnemyAction::Reset() {
	attacked_enemy = false;
}

bool AttackEnemyAction::IsDone() {
	return attacked_enemy;
}
bool AttackEnemyAction::RequiresInRange() {
	return true;
}

bool AttackEnemyAction::CheckProceduralPrecondition(Node *goap_agent, Dictionary world_data) {
	//Has to have seen an enemy recently and it is in RequiresInRange
	TileGrid *tilegrid = cast_to<TileGrid>(goap_agent->call("GetTileGrid"));
	Vector2i ai_location = tilegrid->call("GetCoordinateFromPosition", ((Node3D *)goap_agent->get_parent())->get_position(), tilegrid->call("GetOuterSize"));
	Node3D *closest = nullptr;
	int closest_distance = 1000000;
	if (world_data.has("enemies_in_range")) {
		Array characters = world_data["enemies_in_range"];
		for (int i = 0; i < characters.size(); i++) {
			Node3D *character = cast_to<Node3D>(characters[i]);
			Vector2i character_location = tilegrid->call("GetCoordinateFromPosition", character->get_position(), tilegrid->call("GetOuterSize"));
			int distance = tilegrid->call("CalculateDistance", ai_location, character_location);
			if (closest == nullptr) {
				closest = character;
			} else {
				if (distance < closest_distance) {
					closest = character;
					closest_distance = distance;
				}
			}
		}
	}
  if(closest == nullptr) {
    return false;
  }

  target = closest;
  return true;
}

bool AttackEnemyAction::Perform(Node *goap_agent) {
  UtilityFunctions::print("Performing action");
  return true;
}
