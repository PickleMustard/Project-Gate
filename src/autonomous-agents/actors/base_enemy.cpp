#include "base_enemy.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level-generation/tilegrid.h"
#include "tiles/tile.h"

using namespace godot;

BaseEnemy::BaseEnemy() {
}

BaseEnemy::~BaseEnemy() {
}

void BaseEnemy::_bind_methods() {
}

Dictionary BaseEnemy::GetWorldState() {
	Dictionary world_data{};
  UtilityFunctions::print("Getting world state");
	CheckForEnemies();

  world_data["movement_remaining"] = get_parent()->call("GetDistanceRemaining");
	world_data["seen_enemy"] = past_enemies.size() > 0;
	world_data["has_enemy_within_range"] = known_enemies.size() > 0;
  world_data["enemies_in_range"] = known_enemies;

	return world_data;
}

Dictionary BaseEnemy::CreateGoalState() {
  Dictionary goal_data;
  goal_data["attempting_to_find_enemy"] = true;
  //goal_data["has_enemy_within_range"] = true;

  return goal_data;
}

void BaseEnemy::PlanFailed(Dictionary failed_goal) {}
void BaseEnemy::PlanFound(Dictionary goal, Vector<Ref<GoapAction>> actions) { UtilityFunctions::print("Plan Found"); }
void BaseEnemy::ActionsFinished() {UtilityFunctions::print("Finished action!");}
void BaseEnemy::PlanAborted(Ref<GoapAction> aborter) {}

bool BaseEnemy::MoveAgent(Ref<GoapAction> next_action) {
	//next_action has a target tile to move to
	//move towards it using the unitcontroller
  TypedArray<Node> unitcontrols = get_tree()->get_nodes_in_group("unitcontrol");
  Node *unitcontroller = cast_to<Node>(unitcontrols[0]);

  return true;
}

void BaseEnemy::CheckForEnemies() {
	//Given the current position, call the TileGrid GetTilesInRadius and check for enemies
  UtilityFunctions::print("Checking for Enemies");
	known_enemies.clear();
	SceneTree *tree = get_tree();
  UtilityFunctions::print("1");
	TypedArray<Node> tilegrid = tree->get_nodes_in_group("Tilegrid");
  UtilityFunctions::print("2", tilegrid);
	TileGrid *tilegrid_obj = cast_to<TileGrid>(tilegrid[0]);
  UtilityFunctions::print("Got the TileGrid");
	Vector2i tile_location = tilegrid_obj->call("GetCoordinateFromPosition", ((Node3D *)get_parent())->get_position(), tilegrid_obj->call("GetOuterSize"));
	Ref<Tile> tile = tilegrid_obj->call("FindTileOnGrid", tile_location);
	Array neighbors = tilegrid_obj->call("GetNeighborsInRadius", tile, 10);
  UtilityFunctions::print("Got the neighbors");
	for (int i = 0; i < neighbors.size(); i++) {
		if (neighbors[i].call("HasEnemyOnTile")) {
			known_enemies.push_back(neighbors[i].call("GetCharacterOnTile"));
		}
	}
	for (int i = 0; i < known_enemies.size(); i++) {
		if (past_enemies.has(known_enemies[i])) {
			past_enemies[known_enemies[i]] = (int)past_enemies[known_enemies[i]] + 1;
		} else {
			past_enemies[known_enemies[i]] = 0;
		}
	}
}

void BaseEnemy::AreEnemiesWithinRange() {}
