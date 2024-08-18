#ifndef INTERACTABLE_H
#define INTERACTABLE_H

#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/callable.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "tiles/IStepOnTile.h"
#include "tiles/tile.h"
namespace godot {

class Interactable : public Tile, public IStepOnTile{
  GDCLASS(Interactable, Tile);

public:
  Interactable();
	Interactable(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint8_t type);
  ~Interactable();

  void TileSteppedOnEvent() override;
  void TileSteppedOffEvent() override;

  void AddStepOnEvent(Callable event);
  bool RemoveStepOnEvent(Callable event);
  void AddStepOffEvent(Callable event);
  bool RemoveStepOffEvent(Callable event);

  protected:
  static void _bind_methods();

private:
  TypedArray<Callable> SteppedOnTileCallables;
  TypedArray<Callable> SteppedOffTileCallables;

};
}

#endif
