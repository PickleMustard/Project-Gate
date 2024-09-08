#include "base_enemy.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/templates/hash_map.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/typed_array.hpp"
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

HashMap<String, Variant> BaseEnemy::GetWorldState() {
	HashMap<String, Variant> world_data{};

	world_data.insert("seen_enemy", false);
	world_data.insert("enemy_within_range", false);

	return world_data;
}

  HashMap<String, Variant> BaseEnemy::CreateGoalState() { return HashMap<String, Variant> {};}
  void BaseEnemy::PlanFailed(HashMap<String, Variant> failed_goal) {}
  void BaseEnemy::PlanFound(HashMap<String, Variant> goal, Vector<Ref<GoapAction>> actions) {}
  void BaseEnemy::ActionsFinished() {}
  void BaseEnemy::PlanAborted(Ref<GoapAction> aborter) {}
  bool BaseEnemy::MoveAgent(Ref<GoapAction> next_action) {return true;}

void BaseEnemy::CheckForEnemies() {
	//Given the current position, call the TileGrid GetTilesInRadius and check for enemies
  SceneTree *tree = get_tree();
  TypedArray<Node> tilegrid = tree->get_nodes_in_group("Tilegrid");
  TileGrid *tilegrid_obj = cast_to<TileGrid>(tilegrid[0]);

  Vector2i tile_location = tilegrid_obj->call("GetCoordinateFromPosition", ((Node3D *)get_parent())->get_position(), tilegrid[0].call("GetOuterSize"));
  Ref<Tile> tile = tilegrid_obj->call("FindTileOnGrid", tile_location);
  Array neighbors = tilegrid_obj->call("GetNeighborsInRadius", tile, 10);
}


  void BaseEnemy::AreEnemiesWithinRange() {}
