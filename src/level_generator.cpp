#include "level_generator.h"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "seeded_random_access.h"
#include "tilegrid.h"
#include <sys/types.h>
#include <cstdint>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

void LevelGenerator::_bind_methods() {
}

LevelGenerator::LevelGenerator() {
	m_outerSize = 1.0f;
	m_innerSize = 0.0f;
	m_height = 1.0f;
	m_is_flat_topped = true;
	m_maximum_grid_size = Vector2i(1000, 1000);
}

LevelGenerator::LevelGenerator(const Vector2i &grid_size) {
	m_maximum_grid_size = grid_size;
}

LevelGenerator::LevelGenerator(float outer_size, float inner_size, float height, bool is_flat_topped, const Vector2i &grid_size) {
	m_outerSize = outer_size;
	m_innerSize = inner_size;
	m_height = height;
	m_is_flat_topped = is_flat_topped;
	m_maximum_grid_size = grid_size;
}

LevelGenerator::~LevelGenerator() {
	//Clean up allocated objects
}

/*
 * Administrator function that generates the 3D tiles that make up a level
 *
 * Arguments:
 * root: The pointer to the root Godot node that all children tiles will be appended under
 *
 * Returns:
 * tile_grid: HashMap<String, Tile *>: A HashMap of every instantiated tile object hashed to its q,r location
 */
HashMap<String, Tile *> LevelGenerator::GenerateLevel(TileGrid *root) {
	HashMap<String, Tile *> tile_grid = HashMap<String, Tile *>{};
	Vector<Vector2i> room_centers{};
	m_Room_Tree_Node *rooms_kd_tree = nullptr;

	Vector<uint8_t> tile_bit_map;
	tile_bit_map.resize(m_maximum_grid_size[0] * m_maximum_grid_size[1]);
	tile_bit_map.fill(0);
	int num_of_rooms = 40;
	Vector2i gridCenter(m_maximum_grid_size[0] / 2, m_maximum_grid_size[1] / 2);

	rooms_kd_tree = m_GenerateTileBitMap(tile_bit_map, rooms_kd_tree, num_of_rooms, 0, 3, gridCenter);
	//UtilityFunctions::print(rooms_kd_tree);
	//UtilityFunctions::print(vformat("%d q, %d r", location[0], location[1]));

	Vector<Vector2i> room_neighbors(m_GenerateMST(room_centers, rooms_kd_tree, 2));
	UtilityFunctions::print(room_neighbors.size());
	m_ConnectTiles(tile_bit_map, room_neighbors);
	//
	m_GenerateRoom(tile_bit_map, tile_grid, root);

	return tile_grid;
}

/*
 * Goes through the tile_bit_map and will generate a 3D hexagon tile where the map states there to be one
 *
 * Arguments:
 * tile_map: Reference to the Vector of unsigned bytes that stores data about whether a tile is present at the row,column location
 * grid_of_tiles: HashMap of the tiles name "Hex <q location> <r location>" to the instantiated tile object
 * root: root Godot node that all children get appended under
 *
 * Returns:
 * grid_of_tiles: Tile locations are hashed to the object present at that location
 * root: Tile objects are added as children of the root Godot Node
 * No direct returns
 */
void LevelGenerator::m_GenerateRoom(Vector<uint8_t> &tile_map, HashMap<String, Tile *> grid_of_tiles, TileGrid *root) {
	for (int i = 0; i < tile_map.size(); i++) {
		if (tile_map.get(i) > 0) {
			int q = i / m_maximum_grid_size[1];
			int r = i - (q * m_maximum_grid_size[1]);

			Vector3 location = TileGrid::GetPositionForHexFromCoordinate(Vector2i(q, r), m_outerSize, m_is_flat_topped);
			//
			Tile *new_tile = memnew(Tile(Vector3(0, 0, 0), q, r, m_is_flat_topped, m_outerSize, m_innerSize, m_height));
			//
			grid_of_tiles.insert(vformat("Hex %d,%d", q, r), new_tile);
      //UtilityFunctions::print(vformat("Tile Name: %s", new_tile->get_name()));
			root->add_child(new_tile, true, Node::INTERNAL_MODE_BACK);
			new_tile->set_owner(root->get_owner());
      new_tile->set_name(vformat("Hex %d,%d", q, r));
      new_tile->SetOwner(root->get_owner());
			//root->set_editable_instance(new_tile, true);
			new_tile->SetTilePosition(location);
			//
		}
	}
}

LevelGenerator::m_Best_Neighbors LevelGenerator::m_FindNearest(m_Room_Tree_Node *node, Vector2i goal_room, m_Best_Neighbors best_neighbors, int level) {
	if (node == nullptr) {
		return best_neighbors;
	}
	int curr_distance = m_HexDistance(node->room_center, goal_room);
	int best_distance = m_HexDistance(goal_room, best_neighbors.neighbor_list[0]);
	int second_best_distance = m_HexDistance(goal_room, best_neighbors.neighbor_list[1]);
	//UtilityFunctions::print(vformat("Goal Dist: %d, Best Distance: %d, Second Best Distance: %d", curr_distance, best_distance, second_best_distance));

	m_Room_Tree_Node *good_side = nullptr;
	m_Room_Tree_Node *bad_side = nullptr;

	//UtilityFunctions::print(vformat("Node Center: %d, %d | goal_room: %d, %d", node->room_center[0], node->room_center[1], goal_room[0], goal_room[1]));
	if (node->room_center != goal_room) {
		//UtilityFunctions::print("Here");
		if (curr_distance < second_best_distance) {
			if (curr_distance < best_distance) {
				//UtilityFunctions::print("Least Distance");
				best_neighbors.neighbor_list.set(1, best_neighbors.neighbor_list[0]);
				best_neighbors.neighbor_list.set(0, node->room_center);
			} else {
				best_neighbors.neighbor_list.set(1, node->room_center);
			}
		}
	}

	bool level_is_even = !(level % 2);

	if (level_is_even) { //On even levels, check the q value
		if (goal_room[0] < node->room_center[0]) {
			good_side = node->left_node;
			bad_side = node->right_node;
		} else {
			good_side = node->right_node;
			bad_side = node->left_node;
		}
	} else { //On odd levels, check the r value
		if (goal_room[1] < node->room_center[1]) {
			good_side = node->left_node;
			bad_side = node->right_node;
		} else {
			good_side = node->right_node;
			bad_side = node->left_node;
		}
	}
	//UtilityFunctions::print(vformat("Best Neighbor: %d, %d | Second Best Neighbor: %d, %d", best_neighbors.neighbor_list[0][0], best_neighbors.neighbor_list[0][1], best_neighbors.neighbor_list[1][0], best_neighbors.neighbor_list[1][1]));
	best_neighbors = m_FindNearest(good_side, goal_room, best_neighbors, level++);
	/*
	 * Even levels split the space on the R-axis, Odd levels split the space on the Q-axis
	 * Want to find a possible point on the intersection of the splitting axis
	 * On Even, Take the Current Node's Q and Goal Node's R to find theoretical point
	 * On Odd, Take the Goal Node's Q and the Current Node's R to find the theoretical point
	 * Calculate the distance from this theoretical point to the goal
	 * If theoretical distance is better than second_best_distance, explore the bad side
	 */
	if (level_is_even) {
		Vector2i theoretical_point = Vector2i{ node->room_center[0], goal_room[1] };
		int theoretical_distance = m_HexDistance(goal_room, theoretical_point);
		//UtilityFunctions::print(vformat("Theoretical Distance: %d, Second Best Distance: %d", theoretical_distance, second_best_distance));
		if (theoretical_distance <= second_best_distance) {
			best_neighbors = m_FindNearest(bad_side, goal_room, best_neighbors, level++);
		}
	} else {
		Vector2i theoretical_point = Vector2i{ goal_room[0], node->room_center[1] };
		int theoretical_distance = m_HexDistance(goal_room, theoretical_point);
		//UtilityFunctions::print(vformat("Theoretical Distance: %d, Second Best Distance: %d", theoretical_distance, second_best_distance));
		if (theoretical_distance <= second_best_distance) {
			best_neighbors = m_FindNearest(bad_side, goal_room, best_neighbors, level++);
		}
	}

	return best_neighbors;
}

/*
 * Need to generate some sort of data structure to grab the nearest n neighbors of a room
 * Currently tries to generate an MST which is great for finding the lines that connect all rooms together with the shortest distance
 * Not great for finding all the best neighbors
 * Will experiment with k-d trees for nearest neighbor; seems less complicated than implimenting a quad or oct tree for more benefit
 * Pseudo-code here: https://www.youtube.com/watch?v=nll58oqEsBg
 */
Vector<Vector2i> LevelGenerator::m_GenerateMST(const Vector<Vector2i> &room_centers, m_Room_Tree_Node *root, u_int8_t size) {
	Vector<Vector2i> neighbor_list{};
	//UtilityFunctions::print(vformat("Root: %d, %d | Right: %d, %d", root->room_center[0], root->room_center[1], root->right_node->room_center[0], root->right_node->room_center[1]));
	m_GenerateNeighborsForNode(root, root, neighbor_list, 0);
	return neighbor_list;
}

void LevelGenerator::m_GenerateNeighborsForNode(m_Room_Tree_Node *current_node, m_Room_Tree_Node *root, Vector<Vector2i> &neighbor_list, int level) {
	if (current_node == nullptr) {
		return;
	}
	m_Best_Neighbors neighbors = { { Vector2i{ 1000000, 1000000 }, Vector2i{ 1000000, 1000000 } } };
	neighbors = m_FindNearest(root, current_node->room_center, neighbors, level);
	neighbor_list.push_back(current_node->room_center);
	neighbor_list.push_back(neighbors.neighbor_list[0]);
	neighbor_list.push_back(neighbors.neighbor_list[1]);
	m_GenerateNeighborsForNode(current_node->left_node, root, neighbor_list, level++);
	m_GenerateNeighborsForNode(current_node->right_node, root, neighbor_list, level++);
}

/*
 * Generates all the tile locations in the bit map to hand off to the 3D hexagon generator
 *
 * Arguments:
 * tile_bit_map: Reference to the Vector of unsigned bytes storing data about whether a tile is present at the row,column location
 * room_centers: Reference to the Vector of 2D Integer Vectors storing a room's center tile's row,column position
 * num_of_rooms_remaining: Reference to the # of rooms left available to generate
 * current_level: Describes the level of the tree the generator is currently on
 * max_level: Describes the max level of the tree that the generator can go to
 * max_grid_size: 2D Integer Vector that describes the maximum row, column boundaries of the generator
 *
 * Returns:
 * tile_bit_map: Filled with all locations of tiles
 * root_room: Pointer to the root_room of the k-d tree spanning all generated rooms
 */
LevelGenerator::m_Room_Tree_Node *LevelGenerator::m_GenerateTileBitMap(Vector<uint8_t> &tile_bit_map, m_Room_Tree_Node *root_room, int &num_of_rooms_remaining, int current_level, int max_level, Vector2i max_grid_size) {
	SeededRandomAccess *rnd = SeededRandomAccess::GetInstance();
	int grid_center_q = max_grid_size[0];
	int grid_center_r = max_grid_size[1];
	int random_layer_rooms = rnd->GetInteger(2, 4);
	//int random_layer_rooms = static_cast<int>(Math::round(rnd->GetInteger(5, 8) * (1.0f / (current_level + 1))));
	//int remaining_room_allocation = num_of_rooms_remaining - random_layer_rooms;
	int radius, q_direction, r_direction, q_offset, r_offset;
	int tries = 0;
	do {
		//grid_center_q = max_grid_size[0];
		//grid_center_r = max_grid_size[1];
		radius = rnd->GetInteger(5, 8);
		q_direction = ((rnd->GetWholeNumber(52) % 2) * 2) - 1;
		r_direction = ((rnd->GetWholeNumber(52) % 2) * 2) - 1;
		q_offset = rnd->GetInteger(5, 8);
		r_offset = rnd->GetInteger(5, 8);
		grid_center_q = grid_center_q + q_direction * (5 + radius + q_offset);
		grid_center_r = grid_center_r + r_direction * (5 + radius + r_offset);
		tries++;
	} while ((grid_center_q < 0 ||
					 grid_center_q > m_maximum_grid_size[0] ||
					 grid_center_r < 0 ||
					 grid_center_r > m_maximum_grid_size[1] ||
					 m_OverlappingRooms(tile_bit_map, Vector2i(grid_center_q, grid_center_r), radius)) &&
			(tries < 100));
	//room_centers.push_back(Vector2i(grid_center_q, grid_center_r));
	Vector2i new_room = Vector2i(grid_center_q, grid_center_r);
	if (root_room == nullptr) {
		//UtilityFunctions::print(root_room);
		root_room = new m_Room_Tree_Node{ new_room, nullptr, nullptr };
		//UtilityFunctions::print(root_room);
	} else {
		m_AddNodeToTree(root_room, new_room, 0);
	}
	m_FillBitMap(tile_bit_map, grid_center_q, grid_center_r, radius);
	num_of_rooms_remaining--;

	if (current_level < max_level && num_of_rooms_remaining > 0) {
		//
		if (num_of_rooms_remaining < random_layer_rooms) {
			//
			for (int i = 0; i < num_of_rooms_remaining && num_of_rooms_remaining > 0; i++) {
				m_GenerateTileBitMap(tile_bit_map, root_room, num_of_rooms_remaining, current_level + 1, max_level, Vector2i(grid_center_q, grid_center_r));
			}
		} else {
			//
			for (int i = 0; i < random_layer_rooms; i++) {
				m_GenerateTileBitMap(tile_bit_map, root_room, num_of_rooms_remaining, current_level + 1, max_level, Vector2i(grid_center_q, grid_center_r));
			}
		}
	}
	rnd = nullptr;
	return root_room;
	//
}

/*
 * Add a node to the k-d tree. Checks against q then r.
 */
void LevelGenerator::m_AddNodeToTree(m_Room_Tree_Node *root_room, Vector2i new_room, int level) {
	if (!(level % 2)) {
		if (new_room.x < root_room->room_center.x) {
			if (root_room->left_node == nullptr) {
				root_room->left_node = new m_Room_Tree_Node{ new_room, nullptr, nullptr };
			} else {
				m_AddNodeToTree(root_room->left_node, new_room, level++);
			}
		} else if (new_room.x >= root_room->room_center.x) {
			if (root_room->right_node == nullptr) {
				root_room->right_node = new m_Room_Tree_Node{ new_room, nullptr, nullptr };
			} else {
				m_AddNodeToTree(root_room->right_node, new_room, level++);
			}
		}
	} else {
		if (new_room.y < root_room->room_center.y) {
			if (root_room->left_node == nullptr) {
				root_room->left_node = new m_Room_Tree_Node{ new_room, nullptr, nullptr };
			} else {
				m_AddNodeToTree(root_room->left_node, new_room, level++);
			}
		} else if (new_room.y >= root_room->room_center.y) {
			if (root_room->right_node == nullptr) {
				root_room->right_node = new m_Room_Tree_Node{ new_room, nullptr, nullptr };
			} else {
				m_AddNodeToTree(root_room->right_node, new_room, level++);
			}
		}
	}
}

/*
 * Checks for overlapping tiles in the radius of the room to be generated
 * Checks all r tiles in 6 directions
 *
 * Arguments:
 * tile_bit_map: Constant Reference to the Vector of unsigned bytes storing data about whether a tile is present at the row,column location
 * center: 2D Integer Vector containing the q,r center of the room
 * radius: Integer radius that the hexagonal room will be
 *
 * Returns:
 * overlapping_room_exists: Boolean : Returns true if there is a room that is found to overlap the current one, otherwise returns false
 */
bool LevelGenerator::m_OverlappingRooms(const Vector<uint8_t> &tile_bit_map, Vector2i center, int radius) {
	Vector2i directions[] = { Vector2i(1, 0), Vector2i(1, -1), Vector2i(0, -1),
		Vector2i(-1, 0), Vector2i(-1, 1), Vector2i(0, 1) };
	Vector2i curr_hex = Vector2i(center[0] - radius, center[1] + radius);
	for (int i = 0; i < 6; i++) {
		for (int j = 0; j < radius; j++) {
			//ERR_FAIL_INDEX_V_MSG(curr_hex[0] * _maximum_grid_size[1] + curr_hex[1], tile_bit_map.size(), true, "Exceeds size of bit map" + String::num_int64(_maximum_grid_size[0]) +
			//                    " " + String::num_int64(_maximum_grid_size[1]) + "|" + String::num_int64(curr_hex[0]) + " " + String::num_int64(curr_hex[1]));
			if (curr_hex[0] * m_maximum_grid_size[1] + curr_hex[1] < tile_bit_map.size() &&
					tile_bit_map[curr_hex[0] * m_maximum_grid_size[1] + curr_hex[1]] > 0) {
				return true;
			}
			curr_hex += directions[i];
		}
	}
	return false;
}

/*
 * Fills the bitmap tiles surrounding a known center with a hexagonal pattern of r radius
 *
 * Arguments:
 * tile_bit_map: Reference to the Vector of unsigned bytes storing data about whether a tile is present at the row,column location
 * q_center: q-coordinate of the center of the room
 * r_center: r-coordinate of the cneter of the room
 * radius: radius r of the room to be filled
 *
 * Returns:
 * tile_bit_map: Tiles that should have a 3D model generated are flipped to a value of 1
 * No Direct Returns
 */
void LevelGenerator::m_FillBitMap(Vector<uint8_t> &tile_bit_map, int q_center, int r_center, int radius) {
	for (int q = -radius; q <= radius; q++) {
		int r1 = Math::max(-radius, -q - radius);
		int r2 = Math::min(radius, -q + radius);
		for (int r = r1; r <= r2; r++) {
			tile_bit_map.set((q_center + q) * m_maximum_grid_size[1] + (r_center + r), 1);
		}
	}
}

/*
 * Given the list of room neighbors, draw a line of tiles from room i to its neighbors i+1 and i+2 for every room in room_neighbors
 *
 * Arguments:
 * tile_bit_map: Reference to the Vector of unsigned bytes storing data about whether a tile is present at the row,column location
 * room_neighbors: Vector of 2D Integer Vectors storing room center data in the format of Room A, Room A Neighbor 1, Room A Neighbor 2, Room B,...
 *
 * Returns:
 * tile_bit_map: Tiles that should have a 3D model generated are flipped to a value of 1
 * No direct returns
 */
void LevelGenerator::m_ConnectTiles(Vector<uint8_t> &tile_bit_map, Vector<Vector2i> room_neighbors) {
	for (int i = 0; i < room_neighbors.size() - 1; i += 3) {
		m_DrawLineTiles(tile_bit_map, room_neighbors[i], room_neighbors[i + 1]);
		m_DrawLineTiles(tile_bit_map, room_neighbors[i], room_neighbors[i + 2]);
	}
}

/*
 * Draws a lines of tiles from the first specified room to the second specified room
 *
 * Arguments:
 * tile_bit_map: Reference to the Vector of unsigned bytes storing data about whether a tile is present at the row,column location
 * first_room_center: Center location of the 1st room
 * second_room_center: Center location of the 2nd room
 *
 * Returns:
 *  tile_bit_map: Locations that should have a connecting tile flipped to a value of 1
 *  No direct returns
 */
void LevelGenerator::m_DrawLineTiles(Vector<uint8_t> &tile_bit_map, Vector2i first_room_center, Vector2i second_room_center) {
	int distance = m_HexDistance(first_room_center, second_room_center);
	for (int i = 0; i <= distance; i++) {
		Vector2i location = m_HexRound(first_room_center, second_room_center, distance, i);
		//UtilityFunctions::print(vformat("%d q, %d r", location[0], location[1]));
		if (location[0] >= 0 && location[1] >= 0) {
			tile_bit_map.set(location[0] * m_maximum_grid_size[1] + location[1], 0x0001);
			//tile_bit_map.set(location[0] * _maximum_grid_size[1] + location[1] + 1, 0x0001);
		}
	}
}

Vector2i LevelGenerator::m_HexRound(Vector2i first_room, Vector2i second_room, int distance, int step) {
	Vector2 approximate_location = Vector2(first_room[0] + (second_room[0] - first_room[0]) * (1.0f / distance * step), first_room[1] + (second_room[1] - first_room[1]) * (1.0f / distance * step));
	int q = static_cast<int>(Math::round(approximate_location[0]));
	int r = static_cast<int>(Math::round(approximate_location[1]));

	float q_diff = q - approximate_location[0];
	float r_diff = r - approximate_location[1];

	if (Math::abs(q) >= Math::abs(r)) {
		return Vector2i(q + static_cast<int>(Math::round(q_diff + 0.5f * r_diff)), r);
	} else {
		return Vector2i(q, r + static_cast<int>(Math::round(r_diff * 0.5f * q_diff)));
	}
}

int LevelGenerator::m_HexDistance(Vector2i first_room, Vector2i second_room) {
	int distance = (Math::abs(first_room[0] - second_room[0]) +
						   Math::abs(first_room[0] + first_room[1] -
								   second_room[0] - second_room[1]) +
						   Math::abs(first_room[1] - second_room[1])) /
			2;
	return distance;
}
