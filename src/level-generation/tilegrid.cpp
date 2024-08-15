#include "tilegrid.h"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/core/class_db.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/core/object.hpp"
#include "godot_cpp/core/property_info.hpp"
#include "godot_cpp/templates/hash_set.hpp"
#include "godot_cpp/templates/vector.hpp"
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

Vector2i TileGrid::GetCoordinateFromPosition(Vector3 location, float size) {
	float easy_sqrt = Math::sqrt(3.0f) * size;
	float q = ((2.0f / 3.0f) * location.x) / 3.0f;
	float r = ((-1.0f / 3.0f) * location.x + (Math::sqrt(3.0f) / 3.0f) * (location.z + (easy_sqrt / 2.0f))) / 3.0f;
	return AxialRound(Vector2i(q, r));
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
			m_tile_grid->clear();
			int num_childs = get_child_count();
			TypedArray<Node> children = get_children();

			//for (int i = 0; i < num_childs; i++) {
			//UtilityFunctions::print(children[0].stringify());
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
	UtilityFunctions::print(vformat("Constructing New Grid with %d num rooms", m_grid_num_rooms));
	m_showrooms = memnew(LevelGenerator(m_tile_outer_size, m_tile_inner_size, m_tile_height, m_tile_is_flat_topped, m_grid_num_rooms, Vector2i(1000, 1000)));
	m_tile_grid = m_showrooms->GenerateLevel(this);
	UtilityFunctions::print("Tile Grid Size: ", m_tile_grid->size());
	memdelete(m_showrooms);
}

/*
 * Default Constructor
 * Creates the reference to the grids HashMap
 */
TileGrid::TileGrid() {
	m_tile_is_flat_topped = true;
	m_tile_grid = new HashMap<String, Tile *>{};
}

TileGrid::TileGrid(Vector3 origin, int num_rooms) {
	m_tile_is_flat_topped = true;
	m_tile_grid = new HashMap<String, Tile *>{};
	m_grid_origin = origin;
	m_grid_num_rooms = num_rooms;
}

/*
 * Default Destructor
 * Frees the reference to the grids HashMap
 */
TileGrid::~TileGrid() {
	m_tile_grid->clear();
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
	Tile *found_tile = m_tile_grid->get(vformat("hex %d,%d", location[0], location[1]));
	return found_tile;
}

Vector<Tile *> TileGrid::GetRingToDist(Tile *center_tile, int radius) {
	Vector<Tile *> tiles{};
	tiles.push_back(center_tile);
	for (int q = -radius; q <= radius; q++) {
		int r1 = Math::max(-radius, -q - radius);
		int r2 = Math::min(radius, -q + radius);
		for (int r = r1; r <= r2; r++) {
			String potential_name = vformat("hex %d,%d", center_tile->GetColumn() + q, center_tile->GetRow() + r);
			if (m_tile_grid->has(potential_name)) {
				tiles.push_back(m_tile_grid->get(potential_name));
			}
		}
	}
  return tiles;
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
	String locations[]{
		vformat("hex %d,%d", tile->GetColumn(), tile->GetRow() + 1),
		vformat("hex %d,%d", tile->GetColumn() + 1, tile->GetRow()),
		vformat("hex %d,%d", tile->GetColumn() + 1, tile->GetRow() - 1),
		vformat("hex %d,%d", tile->GetColumn(), tile->GetRow() - 1),
		vformat("hex %d,%d", tile->GetColumn() - 1, tile->GetRow()),
		vformat("hex %d,%d", tile->GetColumn() - 1, tile->GetRow() + 1)
	};

	for (int i = 0; i < 6; i++) {
		if (m_tile_grid->has(locations[i])) {
			neighbors.push_back(m_tile_grid->get(locations[i]));
		}
	}
	return neighbors;
}

Vector2i TileGrid::AxialScale(Vector2i hex, int scale) {
	return Vector2i(hex.x * scale, hex.y * scale);
}

// * Given a position in axial notation, find the closest tile

Vector2i TileGrid::AxialRound(Vector2i hex) {
	return CubeToAxial(CubeRound(AxialToCube(hex)));
	return Vector2(0, 0);
}

/*
 * Round a possible tile position in cube notation to the closest absolute location
 *
 * Parameters:
 * hex: Position of a point in cube notation
 *
 * Returns:
 * Vector3: Position of the closest tile to the point in cube notation
 */
Vector3 TileGrid::CubeRound(Vector3 hex) {
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
Vector2i TileGrid::CubeToAxial(Vector3 hex) {
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
Vector3 TileGrid::AxialToCube(Vector2i hex) {
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
Vector2i TileGrid::AxialToOffset(Vector2i hex) {
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
Vector2i TileGrid::OffsetToAxial(Vector2i hex) {
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

/*
 * Calculate the path a unit should take from its current tile to the desired tile
 * Arguments:
 * starting_location: Vector2i column, row representation of the tile unit is currently on
 * end_location: Vector2i column, row representation of the desired tile to end on
 *
 * Returns:
 * path: Vector of Tile* describing the optimal path to travel for a unit
 *
 * Errors:
 * Empty Vector: Returns an empty vector if the path cannot be calculated (i.e. if tilegrid is split or does not connect)
 */
godot::Array TileGrid::CalculatePath(Vector2i starting_location, Vector2i end_location) {
	Tile *first_node, *wanted_node;
	godot::Vector<Tile *> open_tiles;
	godot::HashSet<Tile *> closed_tiles;
	godot::Vector<Tile *> final_path;
	godot::Array final_path_arr;
	godot::Vector<Tile *> neighbors;

	first_node = FindTileOnGrid(starting_location);
	wanted_node = FindTileOnGrid(end_location);

	open_tiles.push_back(first_node);
	while (open_tiles.size() > 0) {
		Tile *current_tile = open_tiles[0];
		for (int i = 1; i < open_tiles.size(); i++) {
			if (open_tiles[i]->GetFCost() < current_tile->GetFCost() || open_tiles[i]->GetFCost() == current_tile->GetFCost()) {
				if (open_tiles[i]->GetHCost() < current_tile->GetHCost()) {
					current_tile = open_tiles[i];
				}
			}
		}
		open_tiles.erase(current_tile);
		closed_tiles.insert(current_tile);
		if (current_tile == wanted_node) {
			final_path = RetracePath(first_node, wanted_node);
			for (Tile *t : final_path) {
				final_path_arr.append(t);
			}

			open_tiles.clear();
			closed_tiles.clear();
			neighbors.clear();
			return final_path_arr;
		}

		neighbors = GetNeighbors(current_tile);
		for (Tile *neighbor : neighbors) {
			if (neighbor->GetTileType() == 4 || closed_tiles.has(neighbor)) {
				continue;
			}

			int new_cost_to_neighbor = current_tile->GetGCost() + CalculateDistance(current_tile, neighbor);
			if (new_cost_to_neighbor < neighbor->GetGCost() || !open_tiles.has(neighbor)) {
				neighbor->SetGCost(new_cost_to_neighbor);
				neighbor->SetHCost(CalculateDistance(neighbor, wanted_node));
				neighbor->SetParent(current_tile);
			}
			if (!open_tiles.has(neighbor)) {
				open_tiles.push_back(neighbor);
			}
		}
	}
	open_tiles.clear();
	closed_tiles.clear();
	neighbors.clear();
	return final_path_arr;
}

/*
 * Retraces the optimal path into a vector from an end location to the start location
 * Arguments:
 * start_tile: Starting tile that the path should originate from
 * end_tile: Ending tile that path should end on
 *
 * Returns:
 * retraced_path: Path of
 */
godot::Vector<Tile *> TileGrid::RetracePath(Tile *start_tile, Tile *end_tile) {
	Vector<Tile *> retraced_path{};
	Tile *current_tile = end_tile;

	while (current_tile != start_tile) {
		retraced_path.push_back(current_tile);
		current_tile = current_tile->GetParent();
	}
	retraced_path.reverse();
	return retraced_path;
}

int TileGrid::CalculateDistance(Tile *location, Tile *destination) {
	return DistanceHex(location->GetLocation(), destination->GetLocation());
}

Vector2i TileGrid::SubtractHex(Vector2i a, Vector2i b) {
	return Vector2i(a.x - b.x, a.y - b.y);
}

int TileGrid::LengthHex(Vector2i hex) {
	return Math::round((Math::abs(hex.x) + Math::abs(hex.y) + Math::abs(hex.x + hex.y)) / 2.0f);
}

int TileGrid::DistanceHex(Vector2i a, Vector2i b) {
	return LengthHex(SubtractHex(a, b));
}

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

void TileGrid::SetTileHeight(float new_height) {
	m_tile_height = new_height;
}

float TileGrid::GetTileHeight() {
	return m_tile_height;
}

int TileGrid::GetNumRooms() {
	return m_grid_num_rooms;
}

void TileGrid::SetNumRooms(int num_rooms) {
	m_grid_num_rooms = num_rooms;
	GenerateTileGrid();
}

void TileGrid::_bind_methods() {
	godot::ClassDB::bind_static_method("TileGrid", godot::D_METHOD("GetPositionForHexFromCoordinate", "coordinate", "size", "is_flat_topped"), &TileGrid::GetPositionForHexFromCoordinate);
	godot::ClassDB::bind_static_method("TileGrid", godot::D_METHOD("GetCoordinateFromPosition", "position", "size"), &TileGrid::GetCoordinateFromPosition);
	godot::ClassDB::bind_method(godot::D_METHOD("GenerateTileGrid"), &TileGrid::GenerateTileGrid);
	godot::ClassDB::bind_method(godot::D_METHOD("SetOuterSize", "new_size"), &TileGrid::SetOuterSize);
	godot::ClassDB::bind_method(godot::D_METHOD("GetOuterSize"), &TileGrid::GetOuterSize);
	godot::ClassDB::bind_method(godot::D_METHOD("SetInnerSize", "new_size"), &TileGrid::SetInnerSize);
	godot::ClassDB::bind_method(godot::D_METHOD("GetInnerSize"), &TileGrid::GetInnerSize);
	godot::ClassDB::bind_method(godot::D_METHOD("SetFlatTopped", "is_flat"), &TileGrid::SetFlatTopped);
	godot::ClassDB::bind_method(godot::D_METHOD("GetFlatTopped"), &TileGrid::GetFlatTopped);
	godot::ClassDB::bind_method(godot::D_METHOD("SetTileHeight", "new_height"), &TileGrid::SetTileHeight);
	godot::ClassDB::bind_method(godot::D_METHOD("GetTileHeight"), &TileGrid::GetTileHeight);
	godot::ClassDB::bind_method(godot::D_METHOD("SetNumRooms", "num_rooms"), &TileGrid::SetNumRooms);
	godot::ClassDB::bind_method(godot::D_METHOD("GetNumRooms"), &TileGrid::GetNumRooms);

	//godot::ClassDB::bind_method(godot::D_METHOD("CalculateDistance", "Location", "Destination"), &TileGrid::CalculateDistance);
	godot::ClassDB::bind_method(godot::D_METHOD("CalculatePath", "starting_location", "end_location"), &TileGrid::CalculatePath);
	godot::ClassDB::bind_method(godot::D_METHOD("GetNeighbors", "tile"), &TileGrid::GetNeighbors);

	ADD_GROUP("Tile Properties", "m_tile_");
	ADD_PROPERTY(PropertyInfo(Variant::BOOL, "m_tile_is_flat_topped"), "SetFlatTopped", "GetFlatTopped");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_outer_size"), "SetOuterSize", "GetOuterSize");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_inner_size"), "SetInnerSize", "GetInnerSize");
	ADD_PROPERTY(PropertyInfo(Variant::FLOAT, "m_tile_height"), "SetTileHeight", "GetTileHeight");
	ADD_GROUP("Grid Properties", "m_grid_");
	ADD_PROPERTY(PropertyInfo(Variant::INT, "m_grid_num_rooms"), "SetNumRooms", "GetNumRooms");
}
