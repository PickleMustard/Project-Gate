#include "obstacle.h"

godot::Obstacle::Obstacle() :
		Tile() {
}

godot::Obstacle::Obstacle(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint8_t tile_type) :
		Tile(position, r, c, flat_topped, outer_size, inner_size, height, tile_type) {
}

godot::Obstacle::~Obstacle() {
}

void godot::Obstacle::_bind_methods() {

}
