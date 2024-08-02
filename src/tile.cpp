#include "tile.h"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/mesh_instance3d.hpp"
#include "godot_cpp/classes/ref.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/utility_functions.hpp"

using namespace godot;

/*
* Binding Methods to allow use in C#
* Tile_Position allows a specific tile to change location
*/
void Tile::_bind_methods() {
  godot::ClassDB::bind_method(godot::D_METHOD("set_tile_position", "new_pos"), &Tile::SetTilePosition, Vector3());
	godot::ClassDB::bind_method(godot::D_METHOD("SetOuterSize", "new_size"), &Tile::SetOuterSize);
	godot::ClassDB::bind_method(godot::D_METHOD("GetOuterSize"), &Tile::GetOuterSize);
  godot::ClassDB::bind_method(godot::D_METHOD("SetInnerSize", "new_size"), &Tile::SetInnerSize);
	godot::ClassDB::bind_method(godot::D_METHOD("GetInnerSize"), &Tile::GetInnerSize);
  godot::ClassDB::bind_method(godot::D_METHOD("SetFlatTopped", "is_flat"), &Tile::SetFlatTopped);
	godot::ClassDB::bind_method(godot::D_METHOD("GetFlatTopped"), &Tile::GetFlatTopped);
  godot::ClassDB::bind_method(godot::D_METHOD("SetTileHeight", "new_height"), &Tile::SetTileHeight);
	godot::ClassDB::bind_method(godot::D_METHOD("GetTileHeight"), &Tile::GetTileHeight);

	ADD_GROUP("Tile Properties", "m_tile_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "m_tile_is_flat_topped"), "SetFlatTopped", "GetFlatTopped");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_outer_size"), "SetOuterSize", "GetOuterSize");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_inner_size"), "SetInnerSize", "GetInnerSize");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_height"), "SetTileHeight", "GetTileHeight");
}

/*
* Default constructor for a tile; should almost never be called
* Initializes tile to the origin of the room grid
* Size of the tile is set to a default 1.0 units
* Creates the mesh from the default saved tile mesh
* Mesh Instance and Collision Shape set as children of Collision Body
* Collision body set as child of tile
*/
Tile::Tile() {
	//UtilityFunctions::print("Unparametered Constructor");
	this->set_position(Vector3(0.0f, 0.0f, 0.0f));
	m_tile_row = 0;
	m_tile_column = 0;
	m_tile_is_flat_topped = true;
	m_tile_outer_size = 1.0f;
	m_tile_inner_size = 0.0f;
	m_tile_height = 1.0f;
	m_mesh = m_rl->load("res://sphere.tres", "Mesh");
	m_mesh_inst->set_mesh(m_mesh);
	this->set_name(vformat("Hex %d,%d", m_tile_row, m_tile_column));
  m_collision_body->add_child(m_collision_shape);
  m_collision_body->add_child(m_mesh_inst);

  m_mesh_inst->create_convex_collision();
  m_collision_shape->make_convex_from_siblings();

  TypedArray<Node> children = m_mesh_inst->get_children();
  m_mesh_inst->get_child(children[0])->reparent(this, false);
  this->add_child(m_collision_body);

	memdelete(m_rl);
}

/*
* Parametered Tile Constructor; Should be used most often
*
* Parameters:
* Position: Defines the position of the tile in world space
* r: Position of the tile on the R axis
* c: Position of the tile on the Q axis
* flat_topped: Defines the rotation of the tile, if is flat_topped, 1 side of the tile aligned to the camera positions;
*   if not flat_topped, 1 vertex of the tile is aligned to the camera positions
*outer_size: Size of the radius from internal point to external vertex; Defines size of Tile
*inner_size: Size of the radius from internal point to internal vertex; Defines the size of a possible inner hole in the tile
*height: Vertical size of the tile
*
* Functions exactly the same as default constructor
*/
Tile::Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height) {
	//UtilityFunctions::print("Parametered Constructor");
	this->set_position(position);
	m_tile_row = r;
	m_tile_column = c;
	m_tile_is_flat_topped = flat_topped;
	m_tile_outer_size = outer_size;
	m_tile_inner_size = inner_size;
	m_tile_height = height;
	m_mesh = m_rl->load("res://sphere.tres", "Mesh");
	m_mesh_inst->set_mesh(m_mesh);
	this->set_name(vformat("Hex %d,%d", m_tile_row, m_tile_column));

  m_collision_body->add_child(m_collision_shape);
  m_collision_body->add_child(m_mesh_inst);

  m_collision_body->set_ray_pickable(true);
	m_mesh_inst->create_convex_collision();
  m_collision_shape->make_convex_from_siblings();

	TypedArray<Node> children = m_mesh_inst->get_children();
	m_mesh_inst->get_child(children[0])->reparent(this, false);
  this->add_child(m_collision_body);

  //m_collision_shape->_input(const Ref<InputEvent> &event) += NotifyLog();
	memdelete(m_rl);
}

Tile::~Tile() {
  m_collision_shape->queue_free();
  m_mesh_inst->queue_free();
  m_mesh.unref();
  m_collision_body->queue_free();
  this->queue_free();
}

/*
* Sets the owner of the node and its children to the specified owner
* Allows the nodes to appear in the tree view
*
* Parameters: Pointer to the Node object that will own the tile
*
* Sets the owner of the mesh, collision body, and shape to the new owner
*/
void Tile::SetOwner(Node *owner){
  m_mesh_inst->set_owner(owner);
  m_collision_body->set_owner(owner);
  m_collision_shape->set_owner(owner);
  this->set_owner(owner);
}

/*
* Test Function to output to a log
*/
void Tile::NotifyLog() {
  UtilityFunctions::print("Moe Moe Kyun >-<");
}

/*
* Move the tile to a new position in world space
* Doesn't move the tile from its row, column position
*
* Parameters:
* new_pos: Vector3 containing the new position in world space to move the tile to
*/
void Tile::SetTilePosition(Vector3 new_pos) {
	this->set_position(new_pos);
}

/*
* Returns the row, column location of the tile as a Vector2i
*
* Returns:
* Vector2i: column, row
*/
Vector2i Tile::GetLocation() {
	return Vector2i(m_tile_column, m_tile_row);
}

/*
* Return the column position of the tile
*
* Returns:
* int: column position
*/
int Tile::GetColumn() {
	return m_tile_column;
}

/*
* Return the row position of the tile
*
* Returns:
* int: row position
*/
int Tile::GetRow() {
	return m_tile_row;
}

bool Tile::GetFlatTopped() {
  return m_tile_is_flat_topped;
}

void Tile::SetFlatTopped(bool is_flat) {
  m_tile_is_flat_topped = is_flat;
}

float Tile::GetInnerSize() {
  return m_tile_inner_size;
}

void Tile::SetInnerSize(float new_size) {
  m_tile_inner_size = new_size;
}

float Tile::GetOuterSize() {
  return m_tile_outer_size;
}

void Tile::SetOuterSize(float new_size) {
  m_tile_outer_size = new_size;
}

float Tile::GetTileHeight() {
  return m_tile_height;
}

void Tile::SetTileHeight(float new_height) {
  m_tile_height = new_height;
}
