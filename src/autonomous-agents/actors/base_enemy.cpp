#include "base_enemy.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/dictionary.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level-generation/tilegrid.h"
#include "seeded_random_access.h"
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
	world_data["actions_remaining"] = get_parent()->call("GetActionPointsRemaining");
	world_data["weapon_range"] = get_parent()->call("GetOptimalWeaponRange");
	world_data["seen_enemy"] = past_enemies.size() > 0;
	world_data["has_enemy_within_range"] = known_enemies.size() > 0;
	world_data["enemies_in_range"] = known_enemies;

	return world_data;
}

Dictionary BaseEnemy::CreateGoalState() {
	Dictionary goal_data;
	goal_data["targetEnemy"] = true;

	return goal_data;
}

void BaseEnemy::PlanFailed(Dictionary failed_goal) {}
void BaseEnemy::PlanFound(Dictionary goal, Vector<Ref<GoapAction>> actions) { UtilityFunctions::print("Plan Found"); }
void BaseEnemy::ActionsFinished() { UtilityFunctions::print("Finished action!"); }
void BaseEnemy::PlanAborted(Ref<GoapAction> aborter) {}

bool BaseEnemy::MoveAgent(Ref<GoapAction> next_action) {
	UtilityFunctions::print("In data provider move action");
	//next_action has a target tile to move to
	//move towards it using the unitcontroller
	Dictionary world_data = GetWorldState();
	TypedArray<Node> unitcontrols = get_tree()->get_nodes_in_group("UnitControl");
	Node *unit_controller = cast_to<Node>(unitcontrols[2]);
	TypedArray<Node> potential_grid = get_tree()->get_nodes_in_group("Tilegrid");
	TileGrid *tilegrid = cast_to<TileGrid>(potential_grid[0]);
	Vector2i target_location = tilegrid->call("GetCoordinateFromPosition", next_action->target->call("get_position"), tilegrid->call("GetOuterSize"));
	Node3D *unit = cast_to<Node3D>(get_parent());
	Vector2i unit_location = tilegrid->call("GetCoordinateFromPosition", unit->get_position(), tilegrid->call("GetOuterSize"));
	int distance = tilegrid->CalculateDistanceStatic(target_location, unit_location);
	Vector2 direction = unit_location - target_location;
	UtilityFunctions::print("Direction between AI and target: ", direction / direction.length(), "| ", direction);

	//If the distance between the enemy and the target is less than the enemies remaining movement
	//Move to a tile close to the enemy
	if (distance < (int)world_data["movement_remaining"]) {
		UtilityFunctions::print("Don't think I should be in here");

	} else {
		Vector2i desired_location = unit_location + (int)world_data["movement_remaining"] * (-direction / Math::abs(direction.length()));
    UtilityFunctions::print("Current Location: ", unit_location);
		UtilityFunctions::print("Desired Location: ", desired_location, "| Movement Remaining: ", (int)world_data["movement_remaining"]);
		Ref<Tile> possible_tile = tilegrid->FindTileOnGrid(desired_location);
		UtilityFunctions::print("Is that a possible tile: ", possible_tile);
		String potential_formated_tile_name = vformat("/root/Level/Level/%s/Hex %d,%d", tilegrid->get_name(), desired_location[0], desired_location[1]);
    UtilityFunctions::print("Formatted Name: ", potential_formated_tile_name);
		Node *found_tile = get_node_or_null(potential_formated_tile_name);
    UtilityFunctions::print("Node: ", found_tile->get_name());

		//Array destinations = unit_controller->call("GetPotentialDestinations");
		//SeededRandomAccess *instance = SeededRandomAccess::GetInstance();
		//int location = instance->GetWholeNumber(destinations.size() - 1);
		//Dictionary meshDict = destinations[location];
		//Node *tile = cast_to<Node>(meshDict["TileMesh"]);
		unit_controller->call("MoveCharacter", get_parent(), found_tile);
		//return true;
	}

	return true;
	//
	//	UtilityFunctions::print("Performing action: ", GetActionName());
	//	Node *unit_controller = cast_to<Node>(goap_agent->call("GetUnitController"));
	//	Array destinations = unit_controller->call("GetPotentialDestinations");
	//	SeededRandomAccess *instance = SeededRandomAccess::GetInstance();
	//	int location = instance->GetWholeNumber(destinations.size() - 1);
	//	Dictionary meshDict = destinations[location];
	//	Node *tile = cast_to<Node>(meshDict["TileMesh"]);
	//	unit_controller->call("MoveCharacter", tile->get_parent());
}

void BaseEnemy::CheckForEnemies() {
	//Given the current position, call the TileGrid GetTilesInRadius and check for enemies
	UtilityFunctions::print("Checking for Enemies");
	known_enemies.clear();
	SceneTree *tree = get_tree();
	TypedArray<Node> tilegrid = tree->get_nodes_in_group("Tilegrid");
	TileGrid *tilegrid_obj = cast_to<TileGrid>(tilegrid[0]);
	UtilityFunctions::print("Got the TileGrid ", ((Node3D *)get_parent())->get_position());
	Vector2i tile_location = tilegrid_obj->call("GetCoordinateFromPosition", ((Node3D *)get_parent())->get_position(), tilegrid_obj->call("GetOuterSize"));
	Ref<Tile> tile = tilegrid_obj->call("FindTileOnGrid", tile_location);
	Array neighbors = tilegrid_obj->call("GetNeighborsInRadius", tile, 10);
	UtilityFunctions::print("Got the neighbors ", neighbors.size());
	for (int i = 0; i < neighbors.size(); i++) {
		if (neighbors[i].call("HasCharacterOnTile")) {
			UtilityFunctions::print("Found Character on Neighbor");
			Node3D *Character = cast_to<Node3D>(neighbors[i].call("GetCharacterOnTile"));
			if (Character != get_parent() && Character->call("GetCharacterTeam") != get_parent()->call("GetCharacterTeam")) {
				known_enemies.push_back(Character);
			}
		}
	}
	UtilityFunctions::print(known_enemies.size());
	for (int i = 0; i < known_enemies.size(); i++) {
		if (past_enemies.has(known_enemies[i])) {
			past_enemies[known_enemies[i]] = (int)past_enemies[known_enemies[i]] + 1;
		} else {
			past_enemies[known_enemies[i]] = 0;
		}
	}
}

void BaseEnemy::AreEnemiesWithinRange() {}
