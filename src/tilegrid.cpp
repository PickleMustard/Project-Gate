#include "tilegrid.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "godot_cpp/variant/variant.hpp"
#include "level_generator.h"
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/vector3.hpp>

using namespace godot;

/*
 * Function to derive the position in world space for a tile given its row, column coordinates, size, and orientation
 * As the axis of q and r are only offset at 30 degrees from each other, a different method of calculating location is needed
 *  to match the x, y, z coordinate grid of world space
 *Calculation taken from here: https://www.redblobgames.com/grids/hexagons/#distances-cube
 *
 * Parameters:
 * coordinate: row, column coordinates q,r in Vector2i format
 * size: radius of the tile from center to edge vertex
 * is_flat_topped: orientation of the tile, matching side to camera or edge vertex to camera
 *
 * Returns:
 * Vector3: Absolute position of the tile in world space
 */
Vector3 TileGrid::GetPositionForHexFromCoordinate(Vector2i coordinate, float size, bool is_flat_topped) {
	float root3 = sqrt(3);
	int column = coordinate[0];
	int row = coordinate[1];
	float width;
	float height;
	float xPosition;
	float yPosition;
	bool shouldOffset;
	float horizontalDistance;
	float verticalDistance;
	if (!is_flat_topped) {
		shouldOffset = (row % 2) == 0;
		width = root3 * size;
		height = 2.0f * size;

		horizontalDistance = width;
		verticalDistance = height * (3.0f / 4.0f);

		xPosition = (root3 * coordinate[0] + root3 / 2.0f * coordinate[1]) * size;
		yPosition = (3.0f / 2.0f * coordinate[1]) * size;
	} else {
		shouldOffset = (column % 2) == 0;
		width = 2.0f * size;
		height = root3 * size;

		horizontalDistance = width * (3.0f / 4.0f);
		verticalDistance = height;

		xPosition = (3.0f / 2.0f * coordinate[0]) * size;
		yPosition = (root3 / 2.0f * coordinate[0] + root3 * coordinate[1]) * size;
	}

	return Vector3(xPosition, 0, yPosition);
}

/*
 * _notification is an inbuilt Godot function for Nodes
 * Allows nodes to recieve and respond to engine signals
 *
 * Currently used to get information when tile_grid is instantiated into the tree
 * This signals the grid to either reform its current grid or generate a new one
 */
void TileGrid::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		if (this->get_child_count() > 0) {
			UtilityFunctions::print("Children Exist; Erasing and regenerating");
			m_tile_grid.clear();
			int num_childs = get_child_count();
      TypedArray<Node> children = get_children();

			//for (int i = 0; i < num_childs; i++) {
      UtilityFunctions::print(children[0].stringify());
			//}
		}
		//if (m_showrooms == nullptr) {
		//UtilityFunctions::print("Nullptr: Constructing new level");
		//m_showrooms = memnew(LevelGenerator(m_tile_outer_size, m_tile_inner_size, m_tile_height, m_tile_is_flat_topped, Vector2i(1000, 1000)));
		//m_tile_grid = m_showrooms->GenerateLevel(this);
		//}
	}
}

/*
 * Devoted Function to generate the tile_grid
 */
void TileGrid::GenerateTileGrid() {
	UtilityFunctions::print("Nullptr: Constructing new level");
	m_showrooms = memnew(LevelGenerator(m_tile_outer_size, m_tile_inner_size, m_tile_height, m_tile_is_flat_topped, Vector2i(1000, 1000)));
	m_tile_grid = m_showrooms->GenerateLevel(this);
}

/*
 * Default Constructor
 * Creates the reference to the grids HashMap
 */
TileGrid::TileGrid() {
	m_tile_is_flat_topped = true;
	m_tile_grid = HashMap<String, Tile *>{};
}

/*
 * Default Destructor
 * Frees the reference to the grids HashMap
 */
TileGrid::~TileGrid() {
	m_tile_grid.clear();
}

/*
 * Given a column, row location, find the tile, if it exists, and return its reference
 *
 * Parameters:
 * location: Possible location of the tile in column, row format
 *
 * Returns:
 * Tile *: Pointer to the tile, if it exists
 */
Tile *TileGrid::FindTileOnGrid(Vector2i location) {
	Tile *found_tile = m_tile_grid.get(vformat("hex %d,%d", location[0], location[1]));
	return found_tile;
}

/*
 * Given a tile reference, find any and all possible neighbors
 *
 * Parameters:
 * tile: Reference to the tile whose neighbors are desired
 *
 * Returns:
 * Vector<Tile *>: List of references to all neighbors that exist; size range is from 0 - 6 inclusive
 */
Vector<Tile *> TileGrid::GetNeighbors(Tile *tile) {
	Vector<Tile *> neighbors{};
	String locations[]{ vformat("hex %d,%d", tile->GetColumn(), tile->GetRow() + 1), vformat("hex %d,%d", tile->GetColumn() + 1, tile->GetRow()), vformat("hex %d,%d", tile->GetColumn() + 1, tile->GetRow() - 1),
		vformat("hex %d,%d", tile->GetColumn(), tile->GetRow() - 1), vformat("hex %d,%d", tile->GetColumn() - 1, tile->GetRow()), vformat("hex %d,%d", tile->GetColumn() - 1, tile->GetRow() + 1) };

	for (int i = 0; i < 6; i++) {
		if (m_tile_grid.has(locations[i])) {
			neighbors.push_back(m_tile_grid.get(locations[i]));
		}
	}
	return neighbors;
}

/*
 * Given a position in axial notation, find the closest tile

Vector2 axialRound(Vector2 hex) {
	//return cubeToAxial(cubeRound(axialToCube(hex)));
	return Vector2(0, 0);
}
*/

/*
 * Round a possible tile position in cube notation to the closest absolute location
 *
 * Parameters:
 * hex: Position of a point in cube notation
 *
 * Returns:
 * Vector3: Position of the closest tile to the point in cube notation
 */
Vector3 cubeRound(Vector3 hex) {
	float rounded_q = Math::round(hex.x);
	float rounded_r = Math::round(hex.y);
	float rounded_s = Math::round(hex.z);

	float q_diff = Math::abs(rounded_q - hex.x);
	float r_diff = Math::abs(rounded_r - hex.y);
	float s_diff = Math::abs(rounded_s - hex.z);

	if (q_diff > r_diff && q_diff > s_diff) {
		rounded_q = -rounded_r - rounded_s;
	} else if (r_diff >= s_diff) {
		rounded_r = -rounded_q - rounded_s;
	} else {
		rounded_s = -rounded_q - rounded_r;
	}

	return Vector3(rounded_q, rounded_r, rounded_s);
}

/*
 * Convert a tile position from cube notation to axial notation
 *
 * Parameters:
 * hex: Vector3 tile position in cube notation, q, r, s
 *
 * Returns:
 * Vector2: Tile position in axial notation q, r
 */
Vector2 cubeToAxial(Vector3 hex) {
	return Vector2(hex.x, hex.y);
}

/*
 * Convert a tile position from axial notation to cube notation
 *
 * Parameters:
 * hex: Vector2 tile position in axial notation q, r
 *
 * Returns:
 * Vector3: tile position in cube notation q, r, s
 */
Vector3 axialToCube(Vector2 hex) {
	float cube_q = hex.x;
	float cube_r = hex.y;
	float cube_s = -hex.x - hex.y;
	return Vector3(cube_q, cube_r, cube_s);
}

/*
 * Convert tile position from axial q, r notation to column, row notation
 *
 * Parameters:
 * hex: Vector2 axial notation q, r position of tile
 *
 * Returns;
 * Vector2: column, row offset notation position of tile
 */
Vector2 axialToOffset(Vector2 hex) {
	float column = hex.x;
	float row = hex.y + (hex.x - Math::fmod(Math::absf(hex.x), 2.0f)) / 2.0f;

	return Vector2(column, row);
}

/*
 * Convert tile position from column, row offset notation to axial q, r notation
 *
 * Parameters:
 * hex: Vector2 row, column offset notation of tile position
 *
 * Returns:
 * Vector2: q, r axial notation tile position
 */
Vector2 offsetToAxial(Vector2 hex) {
	float q = hex.x;
	float r = hex.y - (hex.x - Math::fmod(Math::absf(hex.x), 2.0f)) / 2.0f;
	return Vector2(q, r);
}

/*
void godot::TileGrid::LayoutGrid() {
	int num_childs = this->get_child_count();
	if(num_childs > 0) {
TypedArray<Node> children = this->get_children();
for(int i = 0; i < num_childs; i++) {
	children[i].
}*/

void TileGrid::SetOuterSize(float new_size) {
	m_tile_outer_size = new_size;
}

float TileGrid::GetOuterSize() {
	return m_tile_outer_size;
}

void TileGrid::SetInnerSize(float new_size) {
	m_tile_inner_size = new_size;
}

float TileGrid::GetInnerSize() {
	return m_tile_inner_size;
}

void TileGrid::SetFlatTopped(bool is_flat) {
  m_tile_is_flat_topped = is_flat;
}

bool TileGrid::GetFlatTopped() {
  return m_tile_is_flat_topped;
}

void TileGrid::SetTileHeight(float new_height){
  m_tile_height = new_height;
}

float TileGrid::GetTileHeight() {
  return m_tile_height;
}

void TileGrid::_bind_methods() {
	godot::ClassDB::bind_static_method("TileGrid", godot::D_METHOD("GetPositionForhexFromCoordinate", "coordinate", "size", "is_flat_topped"), &TileGrid::GetPositionForHexFromCoordinate);
	godot::ClassDB::bind_method(godot::D_METHOD("GenerateTileGrid"), &TileGrid::GenerateTileGrid);
	godot::ClassDB::bind_method(godot::D_METHOD("SetOuterSize", "new_size"), &TileGrid::SetOuterSize);
	godot::ClassDB::bind_method(godot::D_METHOD("GetOuterSize"), &TileGrid::GetOuterSize);
  godot::ClassDB::bind_method(godot::D_METHOD("SetInnerSize", "new_size"), &TileGrid::SetInnerSize);
	godot::ClassDB::bind_method(godot::D_METHOD("GetInnerSize"), &TileGrid::GetInnerSize);
  godot::ClassDB::bind_method(godot::D_METHOD("SetFlatTopped", "is_flat"), &TileGrid::SetFlatTopped);
	godot::ClassDB::bind_method(godot::D_METHOD("GetFlatTopped"), &TileGrid::GetFlatTopped);
  godot::ClassDB::bind_method(godot::D_METHOD("SetTileHeight", "new_height"), &TileGrid::SetTileHeight);
	godot::ClassDB::bind_method(godot::D_METHOD("GetTileHeight"), &TileGrid::GetTileHeight);

	ADD_GROUP("Tile Properties", "m_tile_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "m_tile_is_flat_topped"), "SetFlatTopped", "GetFlatTopped");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_outer_size"), "SetOuterSize", "GetOuterSize");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_inner_size"), "SetInnerSize", "GetInnerSize");
  ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_height"), "SetTileHeight", "GetTileHeight");
}
