#ifndef UNIT_SPAWNER_H
#define UNIT_SPAWNER_H
#include "godot_cpp/classes/wrapped.hpp"
#include "tiles/tile.h"
namespace godot {
class UnitSpawner : public Tile  {
  GDCLASS(UnitSpawner, Tile);

public:
  UnitSpawner();
	UnitSpawner(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint8_t tile_type);
  ~UnitSpawner();

  void SpawnCharacter();

protected:
  void SetSpawnerCallable(Callable func);
  static void _bind_methods();

private:
  Callable m_spawn_entity;


};
}

#endif
