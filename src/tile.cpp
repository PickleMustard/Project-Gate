#include "tile.h"
#include "godot_cpp/variant/utility_functions.hpp"
#include <cstdint>

using namespace godot;


/*
 * Default constructor for a tile; should almost never be called
 * Initializes tile to the origin of the room grid
 * Size of the tile is set to a default 1.0 units
 * Creates the mesh from the default saved tile mesh
 * Mesh Instance and Collision Shape set as children of Collision Body
 * Collision body set as child of tile
 */
Tile::Tile() {
	UtilityFunctions::print("Unparametered Constructor");
	m_tile_row = 0;
	m_tile_column = 0;
	m_tile_is_flat_topped = true;
	m_tile_outer_size = 1.0f;
	m_tile_inner_size = 0.0f;
	m_tile_height = 1.0f;
  m_tile_type = 0;
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
Tile::Tile(Vector3 position, int r, int c, bool flat_topped, float outer_size, float inner_size, float height, uint8_t type) {
	m_tile_row = r;
	m_tile_column = c;
	m_tile_is_flat_topped = flat_topped;
	m_tile_outer_size = outer_size;
	m_tile_inner_size = inner_size;
	m_tile_height = height;
  m_tile_type = type;
}

Tile::~Tile() {
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
	//this->set_position(new_pos);
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

int Tile::GetFCost() {
	return m_g_cost + m_h_cost;
}

int Tile::GetHCost() {
	return m_h_cost;
}

int Tile::GetGCost() {
	return m_g_cost;
}

void Tile::SetGCost(int new_g_cost) {
	m_g_cost = new_g_cost;
}

void Tile::SetHCost(int new_h_cost) {
	m_h_cost = new_h_cost;
}

void Tile::SetParent(Tile *new_parent) {
	m_path_parent = new_parent;
}

Tile *Tile::GetParent() {
	return m_path_parent;
}

uint8_t Tile::GetTileType() {
	return m_tile_type;
}
