#ifndef STARTING_TILE_H
#define STARTING_TILE_H

#include "tiles/tile.h"
namespace godot {
class StartingTile : public Tile {
	GDCLASS(StartingTile, Tile);

public:
	StartingTile();
	StartingTile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t type);
	~StartingTile();

	void TileSteppedOnEvent(godot::Variant) override;
	void TileSteppedOffEvent(godot::Variant) override;

	void SpawnCharacter();

protected:
	static void _bind_methods();
	void SetSpawnerCallable(Callable func);

private:
	Callable m_spawn_entity;
};

} //namespace godot

#endif
