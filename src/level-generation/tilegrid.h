#ifndef TILEGRID_H
#define TILEGRID_H
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/variant/array.hpp"
#pragma once

#include "godot_cpp/variant/vector2i.hpp"
#include "level_generator.h"
#include "tiles/tile.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/core/math.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/vector.hpp>

namespace godot {

class TileGrid : public Node3D {
	GDCLASS(TileGrid, Node3D)
public:
	Vector3 m_grid_origin;
	Vector2i m_grid_size;
	Vector<Ref<Tile>> m_path;
	float m_size = 3.0f;
	int m_radius = 7;
	float m_tile_outer_size = 3.0f;
	float m_tile_inner_size = 0.0f;
	float m_tile_height = 3.0f;
	bool m_tile_is_flat_topped;
	int m_grid_num_rooms = 15;
	HashMap<String, Ref<Tile>> *m_tile_grid;

	TileGrid();
	TileGrid(Vector3 origin, int num_rooms);
	~TileGrid();

	void SetSize(float new_size);
	float GetSize();
	void SetRadius(int new_radius);
	int GetRadius();
	void SetOuterSize(float new_size);
	float GetOuterSize();
	void SetInnerSize(float new_size);
	float GetInnerSize();
	void SetTileHeight(float new_height);
	float GetTileHeight();
	void SetFlatTopped(bool is_ft);
	bool GetFlatTopped();
	void SetNumRooms(int num_rooms);
	int GetNumRooms();
	void SetGridSize(Vector2i new_size);
	Vector2i GetGridSize();
	HashMap<String, Ref<Tile>> GetTileMap();

	Ref<Tile> FindTileOnGrid(Vector2i location);
  Array GetNeighbors(Ref<Tile> tile);
  Array GetNeighborsInRadius(Ref<Tile> tile, int radius);
	static Array GetNeighborsStatic(Tile tile, HashMap<String, Ref<Tile>> tile_grid);
	Vector2 PositionToGrid(Vector3 location);

	godot::Array CalculatePath(godot::Vector2i starting_location, godot::Vector2i end_location);
	godot::Vector<Ref<Tile>> RetracePath(Ref<Tile> start_tile, Ref<Tile> end_tile);
	int CalculateDistance(Ref<Tile> location, Ref<Tile> destination);
	static int CalculateDistanceStatic(Vector2i a, Vector2i b);
	static Vector2i SubtractHex(Vector2i a, Vector2i b);
	static int LengthHex(Vector2i hex);
	static int DistanceHex(Vector2i a, Vector2i b);
	void GenerateTileGrid(bool test_flag);

  void AddEnemyCall(Callable addition);
  void SetEnemiesOnGrid();
  void SetPlayerTeamOnGrid();

	static Vector3 GetPositionForHexFromCoordinate(Vector2i coordinate, float size, bool is_flat_topped);
	static Vector2i GetCoordinateFromPosition(Vector3 location, float size);
	static Vector2i AxialScale(Vector2i hex, int scale);
	static Vector2i AxialRound(Vector2i hex);
	static Vector3 CubeRound(Vector3 hex);
	static Vector2i CubeToAxial(Vector3 hex);
	static Vector3 AxialToCube(Vector2i hex);
	static Vector2i AxialToOffset(Vector2i hex);
	static Vector2i OffsetToAxial(Vector2i hex);

protected:
	void _notification(int p_what);

	static void _bind_methods();

private:
	LevelGenerator *m_showrooms = nullptr;
  Vector<Ref<Tile>> spawnable_locations {};
  Vector<Ref<Tile>> start_locations {};
  Vector<Callable> call_set_enemy_start_positions;
};

} //namespace godot

#endif
