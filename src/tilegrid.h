#ifndef TILEGRID_H
#define TILEGRID_H
#include "godot_cpp/variant/variant.hpp"
#include "godot_cpp/variant/vector2i.hpp"
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
	float m_tile_outer_size = 1.0f;
	float m_tile_inner_size = 0.0f;
	float m_tile_height = 1.0f;
	bool m_tile_is_flat_topped;
	HashMap<String, Tile *> m_tile_grid;

  TileGrid();
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
  void SetGridSize(Vector2i new_size);
  Vector2i GetGridSize();
  HashMap<String, Tile *> GetTileMap();


	Tile *FindTileOnGrid(Vector2i location);
	Vector<Tile *> GetNeighbors(Tile *tile);
	Vector2 PositionToGrid(Vector3 location);
	Vector2 AxialRound(Vector2 hex);
	Vector3 CubeRound(Vector3 hex);
	Vector2 CubeToAxial(Vector3 hex);
	Vector3 AxialToCube(Vector2 hex);
	Vector2 AxialToOffset(Vector2 hex);
	Vector2 OffsetToAxial(Vector2 hex);

  godot::Vector<Tile> CalculatePath(Vector3 starting_location, Vector3 end_location);
  godot::Vector<Tile> RetracePath(Tile start_tile, Tile end_tile);
  int CalculateDistance(Tile location, Tile destination);
  Vector2i SubtractHex(Vector2i a, Vector2i b);
  int LengthHex(Vector2i hex);
  int DistanceHex(Vector2i a, Vector2i b);
  void GenerateTileGrid();

  static Vector3 GetPositionForHexFromCoordinate(Vector2i coordinate, float size, bool is_flat_topped);


protected:
  void _notification(int p_what);
  static void _bind_methods();

private:
  LevelGenerator *m_showrooms = nullptr;
	//void LayoutGrid();
};

} //namespace godot

#endif
