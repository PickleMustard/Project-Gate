#ifndef LEVEL_H
#define LEVEL_H

#include "godot_cpp/classes/node.hpp"
#include "level_generator.h"
#include "tilegrid.h"

namespace godot {
class Level : public Node {
	GDCLASS(Level, Node);

public:
	Level();
	~Level();
	void GenerateLevel();

protected:
	static void _bind_methods();
	void _notification(int p_what);

private:
	LevelGenerator *m_level_generator;
	TileGrid *m_tile_grid;
};
} //namespace godot

#endif
