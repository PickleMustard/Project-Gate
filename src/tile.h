#ifndef TILE_H
#define TILE_H

#include "godot_cpp/classes/area3d.hpp"
#include "godot_cpp/classes/collision_object3d.hpp"
#include "godot_cpp/classes/collision_shape3d.hpp"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "tile_collision.h"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class Tile : public Node3D {
	GDCLASS(Tile, Node3D)
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
	MeshInstance3D *m_mesh_inst = memnew(MeshInstance3D);
	TileCollision *m_collision_body = memnew(TileCollision);
	CollisionShape3D *m_collision_shape = memnew(CollisionShape3D);
	Ref<Mesh> m_mesh = memnew(Mesh);
	ResourceLoader *m_rl = memnew(ResourceLoader);
	void (*TileSelected)(Tile *);

public:
	static void _bind_methods();
	Tile();
	Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height);
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
	void SetOwner(Node *owner);

  void NotifyLog();
};

} //namespace godot

#endif
