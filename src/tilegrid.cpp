#include "tilegrid.h"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/scene_tree.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/typed_array.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "level_generator.h"
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/vector3.hpp>

using namespace godot;

void TileGrid::_bind_methods() {
	//godot::ClassDB::bind_method(godot::D_METHOD("GetPositionForhexFromCoordinate", "coordinate", "size", "is_flat_topped"), &TileGrid::GetPositionForhexFromCoordinate, Vector2i(), float(), bool());
}

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

void TileGrid::_notification(int p_what) {
	if (p_what == NOTIFICATION_READY) {
		if (this->get_child_count() > 0) {
			UtilityFunctions::print("Children Exist; Erasing and regenerating");
			m_tile_grid.clear();
			int num_childs = get_child_count();
			for (int i = 0; i < num_childs; i++) {
				remove_child(get_child(i));
			}
		}
		if (m_showrooms == nullptr) {
			UtilityFunctions::print("Nullptr: Constructing new level");
			m_showrooms = memnew(LevelGenerator(m_outer_size, m_inner_size, m_height, m_is_flat_topped, Vector2i(1000, 1000)));
			m_tile_grid = m_showrooms->GenerateLevel(this);
		}
	}
}

void TileGrid::GenerateTileGrid() {
	m_tile_grid = m_showrooms->GenerateLevel(this);
}

TileGrid::TileGrid() {
	m_is_flat_topped = true;
	m_tile_grid = HashMap<String, Tile *>{};
}

TileGrid::~TileGrid() {
	m_tile_grid.clear();
}

Tile *TileGrid::FindTileOnGrid(Vector2i location) {
	Tile *found_tile = m_tile_grid.get(vformat("hex %d,%d", location[0], location[1]));
	return found_tile;
}

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

Vector2 axialRound(Vector2 hex) {
	//return cubeToAxial(cubeRound(axialToCube(hex)));
	return Vector2(0, 0);
}

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

Vector2 cubeToAxial(Vector3 hex) {
	return Vector2(hex.x, hex.y);
}

Vector3 axialToCube(Vector2 hex) {
	float cube_q = hex.x;
	float cube_r = hex.y;
	float cube_s = -hex.x - hex.y;
	return Vector3(cube_q, cube_r, cube_s);
}

Vector2 axialToOffset(Vector2 hex) {
	float column = hex.x;
	float row = hex.y + (hex.x - Math::fmod(Math::absf(hex.x), 2.0f)) / 2.0f;

	return Vector2(column, row);
}

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
