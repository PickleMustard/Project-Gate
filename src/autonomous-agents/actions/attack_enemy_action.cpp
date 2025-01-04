#include "attack_enemy_action.h"
#include "autonomous-agents/base_components/goap_action.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level-generation/tilegrid.h"

using namespace godot;

AttackEnemyAction::AttackEnemyAction() : GoapAction("AttackEnemyAction", 3.0f) {
  actions_remaining = Callable(this, "HasActionPointsRemaining");
  used_all_actions = Callable(this, "UsedAllActionPoints");
	movement_remaining = Callable(this, "HasMovementRemaining");
	AddPrecondition("seen_enemy", true);
	AddPrecondition("has_enemy_within_range", true);
  AddPrecondition("actions_remaining", actions_remaining);
	AddPrecondition("movement_remaining", movement_remaining);
  AddEffect("targetEnemy", true);
}

AttackEnemyAction::~AttackEnemyAction() {
}

void AttackEnemyAction::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("CheckProceduralPrecondition", "goap_agent", "world_data"), &AttackEnemyAction::CheckProceduralPrecondition);
	godot::ClassDB::bind_method(godot::D_METHOD("Perform", "goap_agent"), &AttackEnemyAction::Perform);
  godot::ClassDB::bind_method(godot::D_METHOD("HasActionPointsRemaining"), &AttackEnemyAction::HasActionPointsRemaining);
  godot::ClassDB::bind_method(godot::D_METHOD("HasMovementRemaining"), &AttackEnemyAction::HasMovementRemaining);
  godot::ClassDB::bind_method(godot::D_METHOD("UsedAllActionPoints"), &AttackEnemyAction::UsedAllActionPoints);
}

void AttackEnemyAction::Reset() {
	attacked_enemy = false;
}

bool AttackEnemyAction::IsDone(Node *goap_agent) {
	return attacked_enemy;
}

bool AttackEnemyAction::InProgress(Node *goap_agent) {
  return false;
}
bool AttackEnemyAction::RequiresInRange() {
	return true;
}

bool AttackEnemyAction::GetInRange(Node *goap_agent, Dictionary world_data) {
	TileGrid *tilegrid = cast_to<TileGrid>(goap_agent->call("GetTileGrid"));
  Vector2i target_location = tilegrid->call("GetCoordinateFromPosition", target->call("get_position"), tilegrid->call("GetOuterSize"));
  Node3D *unit = cast_to<Node3D>(goap_agent->get_parent());
	Vector2i unit_location = tilegrid->call("GetCoordinateFromPosition", unit->get_position(), tilegrid->call("GetOuterSize"));
  int distance = tilegrid->CalculateDistanceStatic(target_location, unit_location);
  UtilityFunctions::print("Is in range: ", distance < (int)world_data["weapon_range"], "| distance: ", distance, "| Weapon Range: ", (int)world_data["weapon_range"]);
  return distance < (int)world_data["weapon_range"];
}

bool AttackEnemyAction::CheckProceduralPrecondition(Node *goap_agent, Dictionary world_data) {
	//Has to have seen an enemy recently and it is in RequiresInRange
  UtilityFunctions::print("Checking ", GetActionName(), " preconditions");
	TileGrid *tilegrid = cast_to<TileGrid>(goap_agent->call("GetTileGrid"));
  Node3D *unit = cast_to<Node3D>(goap_agent->get_parent());
  float outer_size = tilegrid->call("GetOuterSize");
	Vector2i ai_location = tilegrid->call("GetCoordinateFromPosition", unit->get_position(), 3.0f);
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
        closest_distance = distance;
			} else {
				if (distance < closest_distance) {
					closest = character;
					closest_distance = distance;
				}
			}
		}
	}
  if(closest == nullptr) {
    UtilityFunctions::print("Could not find closest");
    return false;
  }

  target = closest;
  return true;
}

bool AttackEnemyAction::HasActionPointsRemaining(int remaining) {
  return remaining > 0;
}

bool AttackEnemyAction::UsedAllActionPoints(int remaining) {
  return remaining == 0;
}

bool AttackEnemyAction::HasMovementRemaining(int remaining) {
  return remaining > 0;
}

bool AttackEnemyAction::Perform(Node *goap_agent) {
	TileGrid *tilegrid = cast_to<TileGrid>(goap_agent->call("GetTileGrid"));
	Vector2i character_location = tilegrid->call("GetCoordinateFromPosition", target->call("get_position"), tilegrid->call("GetOuterSize"));
  UtilityFunctions::print("Targeting Tile at Position: ", character_location);
  Ref<Tile> targeted_tile = tilegrid->call("FindTileOnGrid", character_location);
  goap_agent->get_parent()->call("AttackCharacter", targeted_tile);
  UtilityFunctions::print("Performing action ", GetActionName());
  attacked_enemy = true;
  return true;
}
