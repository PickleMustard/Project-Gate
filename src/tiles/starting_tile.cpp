#include "starting_tile.h"
#include "godot_cpp/variant/utility_functions.hpp"


using namespace godot;
godot::StartingTile::StartingTile() : Tile() {

}


godot::StartingTile::StartingTile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint8_t type) : Tile(position, r, c, flat_topped, outer_size, inner_size, height, type) {

}
void godot::StartingTile::SpawnCharacter() {
  UtilityFunctions::print("Spawning a Character at ", GetLocation());
  m_spawn_entity.call(this);
}

void godot::StartingTile::SetSpawnerCallable(Callable func){
  m_spawn_entity = func;
}

godot::StartingTile::~StartingTile() {

}

void godot::StartingTile::_bind_methods() {
  godot::ClassDB::bind_method(godot::D_METHOD("TileSteppedOnEvent"), &StartingTile::TileSteppedOnEvent);
  godot::ClassDB::bind_method(godot::D_METHOD("TileSteppedOffEvent"), &StartingTile::TileSteppedOffEvent);
  godot::ClassDB::bind_method(godot::D_METHOD("SetSpawnerCallable", "func"), &StartingTile::SetSpawnerCallable);
  godot::ClassDB::bind_method(godot::D_METHOD("SpawnCharacter"), &StartingTile::SpawnCharacter);
}

void godot::StartingTile::TileSteppedOnEvent() {
  UtilityFunctions::print("Hello, you stepped on me!");

}

void godot::StartingTile::TileSteppedOffEvent() {
  UtilityFunctions::print("Goodbye, you stepped off me!");

}
