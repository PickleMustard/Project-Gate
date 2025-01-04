#include "ordinary.h"
#include "godot_cpp/variant/utility_functions.hpp"

godot::Ordinary::Ordinary() : Tile() {

}


godot::Ordinary::Ordinary(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t type) : Tile(position, r, c, flat_topped, outer_size, inner_size, height, type) {

}

godot::Ordinary::~Ordinary() {

}

void godot::Ordinary::_bind_methods() {
  godot::ClassDB::bind_method(godot::D_METHOD("TileSteppedOnEvent"), &Ordinary::TileSteppedOnEvent);
  godot::ClassDB::bind_method(godot::D_METHOD("TileSteppedOffEvent"), &Ordinary::TileSteppedOffEvent);
}

void godot::Ordinary::TileSteppedOnEvent() {
  UtilityFunctions::print("Hello, you stepped on me!");

}

void godot::Ordinary::TileSteppedOffEvent() {
  UtilityFunctions::print("Goodbye, you stepped off me!");

}
