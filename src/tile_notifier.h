#ifndef TILE_NOTIFIER_H
#define TILE_NOTIFIER_H

#include "tile_collision.h"
namespace godot {
class TileNotifier : public godot::Node {
public:
	GDCLASS(TileNotifier, godot::Node);
	TileNotifier();
	~TileNotifier();
	void TileSelected(godot::TileCollision *selected_tile);

	static TileNotifier *getInstance();

protected:
	static void _bind_methods();

private:
	static TileNotifier *g_instance;
};
} //namespace godot

#endif
