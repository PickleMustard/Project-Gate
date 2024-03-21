#include "tilegrid.h"
#include "level_generator.h"
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/variant/vector3.hpp>

using namespace godot;

void TileGrid::_bind_methods() {
	//godot::ClassDB::bind_method(godot::D_METHOD("GetPositionForHexFromCoordinate", "coordinate", "size", "is_flat_topped"), &TileGrid::GetPositionForHexFromCoordinate, Vector2i(), float(), bool());
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
		LevelGenerator *showrooms = memnew(LevelGenerator(outer_size, inner_size, height, is_flat_topped, Vector2i(1000, 1000)));
		tile_grid = showrooms->generateLevel(this);
	}
}

TileGrid::TileGrid() {
    is_flat_topped = true;
    tile_grid = HashMap<String, Tile *>{};
}

TileGrid::~TileGrid() {
    tile_grid.clear();
}


/*
void godot::TileGrid::LayoutGrid() {
	int num_childs = this->get_child_count();
	if(num_childs > 0) {
TypedArray<Node> children = this->get_children();
for(int i = 0; i < num_childs; i++) {
	children[i].
}*/
