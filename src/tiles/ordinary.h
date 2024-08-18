#ifndef ORDINARY_H
#define ORDINARY_H

#include "godot_cpp/classes/wrapped.hpp"
#include "tiles/IStepOnTile.h"
#include "tile.h"
namespace godot {
class Ordinary : public Tile, public IStepOnTile {
  GDCLASS(Ordinary, Tile);

public:
  Ordinary();
	Ordinary(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint8_t type);
  ~Ordinary();

  void TileSteppedOnEvent() override;
  void TileSteppedOffEvent() override;

  protected:
  static void _bind_methods();
};
}

#endif
