#ifndef TILE_NOTIFIER_H
#define TILE_NOTIFIER_H

#include "level-generation/tilegrid.h"
#include "tile_collision.h"
namespace godot {
class TileNotifier : public godot::Node {
	GDCLASS(TileNotifier, godot::Node);

public:
	TileNotifier();
	~TileNotifier();
	void TileSelected(godot::TileCollision *selected_tile);
  void GridCreationNotification(TileGrid* grid);

	static TileNotifier *getInstance();

protected:
	static void _bind_methods();

private:
	static TileNotifier *g_instance;
};
} //namespace godot

#endif
