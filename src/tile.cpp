#include "tile.h"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

void Tile::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("set_tile_position", "new_pos"), &Tile::set_tile_position, Vector3());
}

Tile::Tile() {
	//UtilityFunctions::print("Unparametered Constructor");
	this->set_position(Vector3(0.0f, 0.0f, 0.0f));
	row = 0;
	column = 0;
	is_flat_topped = true;
	outer_size = 1.0f;
	inner_size = 0.0f;
	height = 1.0f;
	mesh = rl->load("res://sphere.tres", "Mesh");
	meshInst->set_mesh(mesh);
	this->add_child(meshInst);
    memdelete(rl);
}

Tile::Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height) {
	//UtilityFunctions::print("Parametered Constructor");
	this->set_position(position);
	row = r;
	column = c;
	is_flat_topped = flat_topped;
	outer_size = outer_size;
	inner_size = inner_size;
	height = height;
	mesh = rl->load("res://sphere.tres", "Mesh");
	meshInst->set_mesh(mesh);
	this->add_child(meshInst);
    memdelete(rl);
}

void Tile::set_tile_position(Vector3 new_pos) {
    this->set_position(new_pos);
}
