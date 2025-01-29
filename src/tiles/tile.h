#ifndef TILE_H
#define TILE_H

#include "godot_cpp/classes/collision_object3d.hpp"
#include "godot_cpp/classes/resource.hpp"
#include "godot_cpp/classes/wrapped.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "tiles/IStepOnTile.h"
#include <cstdint>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class Tile : public Resource, IStepOnTile {
	GDCLASS(Tile, Resource);

private:
	Vector3 m_position;
	int m_tile_row;
	int m_tile_column;
	int m_g_cost;
	int m_h_cost;
	bool m_tile_is_flat_topped;
	float m_tile_outer_size;
	float m_tile_inner_size;
	float m_tile_height;
	uint16_t m_tile_type; //Defines the type of the tile walkable, interacable, obstacle, wall
	Node3D *m_entity_on_tile;
	Ref<Tile> m_path_parent;
	void (*TileSelected)(Tile *);

public:
	Tile();
	Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint16_t type);
	~Tile();

	Vector2i GetLocation();
	void SetLocation(Vector2i new_location);
	int GetRow();
	void SetRow(int new_row);
	int GetColumn();
	void SetColumn(int new_row);
	bool GetFlatTopped();
	void SetFlatTopped(bool is_flat);
	float GetOuterSize();
	void SetOuterSize(float new_size);
	float GetInnerSize();
	void SetInnerSize(float new_size);
	float GetTileHeight();
	void SetTileHeight(float new_height);
	Vector3 GetTilePosition();
	void SetTilePosition(Vector3 new_pos);
	void SetGCost(int new_g_cost);
	int GetGCost();
	void SetHCost(int new_h_cost);
	int GetHCost();
	int GetFCost();
	uint8_t GetTileType();
	Ref<Tile> GetParent();
	void SetParent(Ref<Tile> parent);
	void SetCharacterOnTile(Variant character);
	Variant GetCharacterOnTile();
	bool HasCharacterOnTile();
	void ResetCharacterOnTile();
  //Override this method in children to create different behavior in TileSteppedOnEvent
  void InitialEventConnector();
  //Override these two methods for different behavior on Step events
	void TileSteppedOnEvent(godot::Variant entity) override;
	void TileSteppedOffEvent(godot::Variant entity) override;
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

} //namespace godot

#endif
