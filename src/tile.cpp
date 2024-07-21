#include "tile.h"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

void Tile::_bind_methods() {
	godot::ClassDB::bind_method(godot::D_METHOD("set_tile_position", "new_pos"), &Tile::SetTilePosition, Vector3());
}

Tile::Tile() {
	//UtilityFunctions::print("Unparametered Constructor");
	this->set_position(Vector3(0.0f, 0.0f, 0.0f));
	m_row = 0;
	m_column = 0;
	m_is_flat_topped = true;
	m_outer_size = 1.0f;
	m_inner_size = 0.0f;
	m_height = 1.0f;
	m_mesh = m_rl->load("res://sphere.tres", "Mesh");
	m_mesh_inst->set_mesh(m_mesh);
	this->set_name(vformat("Hex %d,%d", m_row, m_column));
	this->add_child(m_mesh_inst);
  this->add_child(m_collision_body);
  m_collision_body->create_shape_owner(m_mesh_inst);
	//m_mesh_inst->set_owner(this);
	m_mesh_inst->create_convex_collision();
	TypedArray<Node> children = m_mesh_inst->get_children();
	m_mesh_inst->get_child(children[0])->reparent(this, false);
	//UtilityFunctions::print(meshInst->get_child_count());
	memdelete(m_rl);
}

Tile::Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height) {
	//UtilityFunctions::print("Parametered Constructor");
	this->set_position(position);
	m_row = r;
	m_column = c;
	m_is_flat_topped = flat_topped;
	m_outer_size = outer_size;
	m_inner_size = inner_size;
	m_height = height;
	m_mesh = m_rl->load("res://sphere.tres", "Mesh");
	m_mesh_inst->set_mesh(m_mesh);
	this->set_name(vformat("Hex %d,%d", m_row, m_column));
	this->add_child(m_mesh_inst);
  this->add_child(m_collision_body);
  m_collision_body->create_shape_owner(m_mesh_inst);
  m_collision_body->set_ray_pickable(true);
	//m_mesh_inst->set_owner(this);
	m_mesh_inst->create_convex_collision();
	TypedArray<Node> children = m_mesh_inst->get_children();
	m_mesh_inst->get_child(children[0])->reparent(this, false);
	//UtilityFunctions::print(meshInst->get_child_count());
	memdelete(m_rl);
}

void Tile::SetOwner(Node *owner){
  m_mesh_inst->set_owner(owner);
  m_collision_body->set_owner(owner);
}

void Tile::NotifyLog() {
  UtilityFunctions::print("Moe Moe Kyun >-<");
}

void Tile::SetTilePosition(Vector3 new_pos) {
	this->set_position(new_pos);
}

Vector2i Tile::GetLocation() {
	return Vector2i(m_column, m_row);
}

int Tile::GetColumn() {
	return m_column;
}

int Tile::GetRow() {
	return m_row;
}
