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
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/variant/string.hpp"
#include "level-generation/tile_mesh_generator.h"
#include "tiles/tile.h"

namespace godot {

class TileGrid;

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

  struct m_Room_Vertex;

  struct m_Room_Edge {
    int weight;
    int constraint;
    Vector2i direction;
    m_Room_Vertex *destination;
  };

  struct m_Room_Vertex {
    int position;
    int room_shape;
    int radius;
    int purpose;
    int interactable_points;
    char32_t color;
    Vector2i bounding_zone;
    Vector2i location;
    godot::HashMap<String, m_Room_Edge *> edges;
  };

  struct m_Rooms_Graph {
    godot::HashMap<String, m_Room_Vertex *> vertices;
  };

	struct m_Best_Neighbors {
		Vector<Vector2i> neighbor_list;
	};

  struct m_Room_Graph_Node {
    Vector<m_Room_Graph_Node> parents;
    Vector<m_Room_Graph_Node> children;
    int shape;
    int no_touchy_space_radius;
    Vector2i node_center;
    Vector<Vector2i> direction;
  };

	float m_outer_size;
	float m_inner_size;
	float m_height;
	bool m_is_flat_topped;
	Vector2i m_maximum_grid_size;
  int m_num_rooms;
  int m_level_point_total; //Defines the # of points a level can use to generate interactable items

protected:
	static void _bind_methods();

public:
	LevelGenerator();
	LevelGenerator(const Vector2i &grid_size);
	LevelGenerator(float outer_size, float inner_size, float height, bool is_flat_topped, int num_rooms, const Vector2i &grid_size);
	~LevelGenerator();

	HashMap<String, Ref<Tile>> *GenerateLevel(TileGrid *root, Vector<Ref<Tile>> &spawnable_locations, String file);
  int GetNumRooms();

private:
	void m_GenerateRoom(Vector<uint16_t> &tile_map, HashMap<String, Ref<Tile>> *grid_of_tiles, TileGrid *root, Vector<Ref<Tile>> &spawnable_locations);
  m_Rooms_Graph* m_GenerateRoomGraph(Vector2i starting_location, String file);
  void m_ReplaceNodesInPattern(m_Rooms_Graph *rooms_graph);
  void m_GenerateGraphTileBitMap(Vector<uint16_t> &tile_bit_map, m_Rooms_Graph *graph, Vector2i grid_origin);
  void m_ConnectGraphNodes(Vector<uint16_t> &tile_bilt_map, m_Rooms_Graph *graph);
  Ref<Tile> m_InstantiateTile(Object* GenerationComunicator, Vector<Ref<Tile>> &spawnable_locations, Vector3 location, int q, int r, uint16_t tile_type);
  void m_SetTileMeshAndMaterial(TileMeshGenerator *mesh_generator, ResourceLoader *rl, String mesh_material_name, String tile_mesh_name, uint16_t tile_type);
  Vector<int> m_GenerateInteractableType(int num_points);
	Vector<Vector2i> m_GenerateMST(const Vector<Vector2i> &room_centers, m_Room_Tree_Node *root, u_int8_t size);
	void m_GenerateNeighborsForNode(m_Room_Tree_Node *current_node, m_Room_Tree_Node *root, Vector<Vector2i> &neighbor_list, int level);
	m_Best_Neighbors m_FindNearest(m_Room_Tree_Node *node, Vector2i goal_room, m_Best_Neighbors best_neighbor, int level);
	m_Room_Tree_Node *m_GenerateTileBitMap(Vector<uint16_t> &tile_bit_map, m_Room_Tree_Node *root_room /*Vector<Vector2i> &room_centers*/, int &num_of_rooms_remaining,
			int current_level, int max_level, Vector2i max_grid_size);
	bool m_OverlappingRooms(const Vector<uint16_t> &tile_bit_map, Vector2i center, int radius);
	void m_FillBitMap(Vector<uint16_t> &tile_bit_map, int q_center, int r_center, int radius);
	void m_ConnectTiles(Vector<uint16_t> &tile_bit_map, Vector<Vector2i> room_neighbors);
	void m_DrawLineTiles(Vector<uint16_t> &tile_bit_map, Vector2i first_room_center, Vector2i second_room_center);
	void m_AddNodeToTree(m_Room_Tree_Node *root_room, Vector2i new_room, int level);
	Vector2i m_HexRound(Vector2i first_room, Vector2i second_room, int distance, int step);
	int m_HexDistance(Vector2i first_room, Vector2i second_room);
};
} //namespace godot
#endif
