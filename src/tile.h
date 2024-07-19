#ifndef TILE_H
#define TILE_H

#include "godot_cpp/classes/collision_object3d.hpp"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector3.hpp>

namespace godot {

class Tile : public Node3D {
	GDCLASS(Tile, Node3D)
private:
	Vector3 position;
	int row;
	int column;
	int g_cost;
	int h_cost;
	bool is_flat_topped;
	float outer_size;
	float inner_size;
	float height;
	MeshInstance3D *meshInst = memnew(MeshInstance3D);
    //CollisionObject3D *collision_detection = memnew(CollisionObject3D);
	Ref<Mesh> mesh = memnew(Mesh);
	ResourceLoader *rl = memnew(ResourceLoader);

public:
	static void _bind_methods();
	Tile();
	Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height);
    Vector2i getLocation();
    int getRow();
    int getColumn();
    void set_tile_position(Vector3 new_pos);
};

} //namespace godot

#endif
