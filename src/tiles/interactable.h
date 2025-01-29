#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "tiles/tile.h"
namespace godot {

class Interactable : public Tile {
  GDCLASS(Interactable, Tile);

public:
  Interactable();
	Interactable(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t tile_type, int interactable_type);
  ~Interactable();

  void TileSteppedOnEvent(godot::Variant entity) override;
  void TileSteppedOffEvent(godot::Variant entity) override;

  void AddStepOnEvent(Callable event);
  bool RemoveStepOnEvent(Callable event);
  void AddStepOffEvent(Callable event);
  bool RemoveStepOffEvent(Callable event);

  protected:
  static void _bind_methods();

private:
  TypedArray<Callable> SteppedOnTileCallables;
  TypedArray<Callable> SteppedOffTileCallables;

  int m_interactable_type;

};
}

#endif
