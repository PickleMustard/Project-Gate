#ifndef TILEGRID_H
#define TILEGRID_H
#pragma once

#include "tile.h"
#include "level_generator.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class TileGrid : public Node3D {
	GDCLASS(TileGrid, Node3D)
public:
	Vector2i m_grid_size;
	Vector<Tile> m_path;
	float m_size = 3.0f;
	int m_radius = 7;
	float m_outer_size = 1.0f;
	float m_inner_size = 0.0f;
	float m_height = 1.0f;
	bool m_is_flat_topped;
	HashMap<String, Tile *> m_tile_grid;

	Tile *FindTileOnGrid(Vector2i location);
	Vector<Tile *> GetNeighbors(Tile *tile);
	static Vector3 GetPositionForHexFromCoordinate(Vector2i coordinate, float size, bool is_flat_topped);
	Vector2 PositionToGrid(Vector3 location);
	Vector2 AxialRound(Vector2 hex);
	Vector3 CubeRound(Vector3 hex);
	Vector2 CubeToAxial(Vector3 hex);
	Vector3 AxialToCube(Vector2 hex);
	Vector2 AxialToOffset(Vector2 hex);
	Vector2 OffsetToAxial(Vector2 hex);
	void _notification(int p_what);
  void GenerateTileGrid();
	TileGrid();
	~TileGrid();

protected:
	static void _bind_methods();

private:
  LevelGenerator *m_showrooms = nullptr;
	//void LayoutGrid();
};

} //namespace godot

#endif
