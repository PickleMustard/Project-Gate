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
	int m_row;
	int m_column;
	int m_g_cost;
	int m_h_cost;
	bool m_is_flat_topped;
	float m_outer_size;
	float m_inner_size;
	float m_height;
	MeshInstance3D *m_mesh_inst = memnew(MeshInstance3D);
  //StaticBody3D *m_collision_body = memnew(StaticBody3D);
  TileCollision *m_collision_body = memnew(TileCollision);
  CollisionShape3D *m_collision_shape = memnew(CollisionShape3D);
	Ref<Mesh> m_mesh = memnew(Mesh);
	ResourceLoader *m_rl = memnew(ResourceLoader);

public:
	static void _bind_methods();
	Tile();
	Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height);
  ~Tile();
	Vector2i GetLocation();
	int GetRow();
	int GetColumn();
	void SetTilePosition(Vector3 new_pos);
	void SetOwner(Node *owner);
  void NotifyLog();
};

} //namespace godot

#endif
