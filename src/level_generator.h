#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/list.hpp>
#include <godot_cpp/templates/vector.hpp>
#include <godot_cpp/variant/vector2i.hpp>

#include <sys/types.h>
#include <tile.h>
#include <tilegrid.h>

namespace godot {

class LevelGenerator : public Object {
	GDCLASS(LevelGenerator, Object);

private:
	struct m_Room {
		Vector2i room_center;
		Vector<Vector2i> room_neighbors;
		int level;
		bool visited;
	};

	struct m_Room_Tree_Node {
		Vector2i room_center;
		m_Room_Tree_Node *left_node;
		m_Room_Tree_Node *right_node;
	};

	struct m_Best_Neighbors {
		Vector<Vector2i> neighbor_list;
	};

	float m_outerSize;
	float m_innerSize;
	float m_height;
	bool m_is_flat_topped;
	Vector2i m_maximum_grid_size;

protected:
	static void _bind_methods();

public:
	LevelGenerator();
	LevelGenerator(const Vector2i &grid_size);
	LevelGenerator(float outer_size, float inner_size, float height, bool is_flat_topped, const Vector2i &grid_size);
	~LevelGenerator();

	HashMap<String, Tile *> GenerateLevel(TileGrid *root);

private:
	void m_GenerateRoom(Vector<uint8_t> &tile_map, HashMap<String, Tile *> grid_of_tiles, TileGrid *root);
	Vector<Vector2i> m_GenerateMST(const Vector<Vector2i> &room_centers, m_Room_Tree_Node *root, u_int8_t size);
	void m_GenerateNeighborsForNode(m_Room_Tree_Node *current_node, m_Room_Tree_Node *root, Vector<Vector2i> &neighbor_list, int level);
	m_Best_Neighbors m_FindNearest(m_Room_Tree_Node *node, Vector2i goal_room, m_Best_Neighbors best_neighbor, int level);
	m_Room_Tree_Node *m_GenerateTileBitMap(Vector<uint8_t> &tile_bit_map, m_Room_Tree_Node *root_room /*Vector<Vector2i> &room_centers*/, int &num_of_rooms_remaining,
			int current_level, int max_level, Vector2i max_grid_size);
	bool m_OverlappingRooms(const Vector<uint8_t> &tile_bit_map, Vector2i center, int radius);
	void m_FillBitMap(Vector<uint8_t> &tile_bit_map, int q_center, int r_center, int radius);
	void m_ConnectTiles(Vector<uint8_t> &tile_bit_map, Vector<Vector2i> room_neighbors);
	void m_DrawLineTiles(Vector<uint8_t> &tile_bit_map, Vector2i first_room_center, Vector2i second_room_center);
	void m_AddNodeToTree(m_Room_Tree_Node *root_room, Vector2i new_room, int level);
	Vector2i m_HexRound(Vector2i first_room, Vector2i second_room, int distance, int step);
	int m_HexDistance(Vector2i first_room, Vector2i second_room);
};
} //namespace godot
#endif
