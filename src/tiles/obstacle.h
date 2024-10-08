#ifndef OBSTACLE_H
#define OBSTACLE_H
#include "godot_cpp/classes/wrapped.hpp"
#include "tiles/tile.h"
namespace godot {
class Obstacle : public Tile {
	GDCLASS(Obstacle, Tile)
public:
  Obstacle();
	Obstacle(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t tile_type);
  ~Obstacle();

  protected:
  static void _bind_methods();
};

} //namespace godot

#endif
