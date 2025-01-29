#ifndef ORDINARY_H
#define ORDINARY_H

#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "tiles/IStepOnTile.h"
#include "tile.h"
namespace godot {
class Ordinary : public Tile {
  GDCLASS(Ordinary, Tile);

public:
  Ordinary();
	Ordinary(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t type);
  ~Ordinary();

  void InitialEventConnector();
  void TileSteppedOnEvent(godot::Variant) override;
  void TileSteppedOffEvent(godot::Variant) override;

  protected:
  static void _bind_methods();
};
}

#endif
