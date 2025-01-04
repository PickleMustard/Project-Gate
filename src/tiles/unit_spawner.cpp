#include "unit_spawner.h"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"


godot::UnitSpawner::UnitSpawner() :
		Tile() {
}

godot::UnitSpawner::UnitSpawner(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t tile_type) :
		Tile(position, r, c, flat_topped, outer_size, inner_size, height, tile_type) {
}

godot::UnitSpawner::~UnitSpawner() {
}

void godot::UnitSpawner::SpawnCharacter() {
  UtilityFunctions::print("Spawning an Enemy at ", GetLocation());
  m_spawn_entity.call(this);
}

void godot::UnitSpawner::SetSpawnerCallable(Callable func){
  //UtilityFunctions::print("called set spawner: ", func);
  m_spawn_entity = func;
}

void godot::UnitSpawner::_bind_methods() {
  godot::ClassDB::bind_method(godot::D_METHOD("SetSpawnerCallable", "func"), &UnitSpawner::SetSpawnerCallable);
  godot::ClassDB::bind_method(godot::D_METHOD("SpawnCharacter"), &UnitSpawner::SpawnCharacter);
}
