#ifndef TILEGRID_H
#define TILEGRID_H

#include "tile.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class TileGrid : public Node3D {
	GDCLASS(TileGrid, Node3D)
public:
	Vector2i grid_size;
	Vector<Tile> path;
	float size = 3.0f;
	int radius = 7;
	float outer_size = 1.0f;
	float inner_size = 0.0f;
	float height = 1.0f;
	bool is_flat_topped;
	HashMap<String, Tile *> tile_grid;

	//Tile findTileOnGrid(Vector2i location);
	//Vector<Tile> GetNeighbors(Tile tile);
	static Vector3 GetPositionForHexFromCoordinate(Vector2i coordinate, float size, bool is_flat_topped);
	//Vector2 positionToGrid(Vector3 location);
	//Vector2 axialRound(Vector2 hex);
	//Vector3 cubeRound(Vector3 hex);
	//Vector2 cubeToAxial(Vector3 hex);
	//Vector3 axialToCube(Vector2 hex);
	//Vector2 axialToOffset(Vector2 hex);
	//Vector2 offsetToAxial(Vector2 hex);
	void _notification(int p_what);
	TileGrid();
	~TileGrid();

protected:
	static void _bind_methods();

private:
	//void LayoutGrid();
};

} //namespace godot

#endif
