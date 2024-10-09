#include "level-generation/level_generator.h"
#include "godot_cpp/classes/collision_shape3d.hpp"
#include "godot_cpp/classes/engine.hpp"
#include "godot_cpp/classes/json.hpp"
#include "godot_cpp/classes/mesh.hpp"
#include "godot_cpp/classes/node.hpp"
#include "godot_cpp/classes/resource_loader.hpp"
#include "godot_cpp/classes/resource_saver.hpp"
#include "godot_cpp/classes/shader_material.hpp"
#include "godot_cpp/core/math.hpp"
#include "godot_cpp/core/memory.hpp"
#include "godot_cpp/variant/string.hpp"
#include "godot_cpp/variant/utility_functions.hpp"
#include "seeded_random_access.h"
#include "tile_collision.h"
#include "tile_mesh_generator.h"
#include "tilegrid.h"
#include "tiles/interactable.h"
#include "tiles/obstacle.h"
#include "tiles/ordinary.h"
#include "tiles/starting_tile.h"
#include "tiles/unit_spawner.h"
#include "yaml/yaml_parser.h"
#include <sys/types.h>
#include <cstdint>
#include <godot_cpp/templates/vector.hpp>

using namespace godot;

void LevelGenerator::_bind_methods() {
}

LevelGenerator::LevelGenerator() {
	m_outer_size = 1.0f;
	m_inner_size = 0.0f;
	m_height = 1.0f;
	m_is_flat_topped = true;
	m_maximum_grid_size = Vector2i(1000, 1000);
}

LevelGenerator::LevelGenerator(const Vector2i &grid_size) {
	m_maximum_grid_size = grid_size;
}

LevelGenerator::LevelGenerator(float outer_size, float inner_size, float height, bool is_flat_topped, int num_rooms, const Vector2i &grid_size) {
	m_outer_size = outer_size;
	m_inner_size = inner_size;
	m_height = height;
	m_is_flat_topped = is_flat_topped;
	m_maximum_grid_size = grid_size;
	m_num_rooms = num_rooms;
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
HashMap<String, Ref<Tile>> *LevelGenerator::GenerateLevel(TileGrid *root, Vector<Ref<Tile>> &spawnable_locations, String file) {
	HashMap<String, Ref<Tile>> *tile_grid = new HashMap<String, Ref<Tile>>{};
	Vector<Vector2i> room_centers{};
	m_Room_Tree_Node *rooms_kd_tree = nullptr;

	Vector<uint16_t> tile_bit_map;
	tile_bit_map.resize(m_maximum_grid_size[0] * m_maximum_grid_size[1]);
	tile_bit_map.fill(0);
	Vector2i gridCenter(m_maximum_grid_size[0] / 2, m_maximum_grid_size[1] / 2);

	//rooms_kd_tree = m_GenerateTileBitMap(tile_bit_map, rooms_kd_tree, m_num_rooms, 0, 3, gridCenter);

	//Vector<Vector2i> room_neighbors(m_GenerateMST(room_centers, rooms_kd_tree, 2));
	//m_ConnectTiles(tile_bit_map, room_neighbors);
	//
	UtilityFunctions::print("Generating Rooms");
	m_Rooms_Graph *rooms_graph = m_GenerateRoomGraph(gridCenter, file);
	UtilityFunctions::print("Generating Bitmap");
	m_ConnectGraphNodes(tile_bit_map, rooms_graph);
	m_GenerateGraphTileBitMap(tile_bit_map, rooms_graph, gridCenter);
	UtilityFunctions::print("Generated Bitmap");
	m_GenerateRoom(tile_bit_map, tile_grid, root, spawnable_locations);
	UtilityFunctions::print("Tile Grid Before Return: ", tile_grid->size());

	return tile_grid;
}

/* Generates the Directed Graph of rooms to create the TileGrid from
 *
 * Arguments:
 * starting_location: Vector2i for the position of the origin on the grid
 *
 * Returns:
 * m_Rooms_Graph Pointer: The pointer to the starting room node within the directed graph
 * 			Will spawn the player units on this tile
 */
LevelGenerator::m_Rooms_Graph *LevelGenerator::m_GenerateRoomGraph(Vector2i starting_location, String file) {
	SeededRandomAccess *rnd = SeededRandomAccess::GetInstance();
	LevelGenerator::m_Rooms_Graph *rooms_graph = new m_Rooms_Graph{ HashMap<String, m_Room_Vertex *>{} };
	UtilityFunctions::print("Parsing File: ", file);
	Dictionary graph_to_build = YamlParser::parse_file(file);
	UtilityFunctions::print("Found the file to parse");
	Dictionary level_metadata = graph_to_build["Level"];
	m_level_point_total = level_metadata["generation_point_total"];
	Array Nodes = graph_to_build["Nodes"];
	m_num_rooms = Nodes.size();
	Array Edges = graph_to_build["Edges"];
	//Generate the vertices
	for (int i = 0; i < Nodes.size(); i++) {
		Dictionary node = Nodes[i];
		String hash_name = vformat("node_%d", i);
		Vector2i no_touchy_space;

		Dictionary node_meta = node[hash_name];
		Array room_bounding_zone = node_meta["room_bounding_zone"];
		Array size_constraints = node_meta["room_size_extents"];
		Array color_possibilities = node_meta["color_possibilities"];
		String room_color = color_possibilities[rnd->GetInteger(0, color_possibilities.size() - 1)];
		int room_shape = node_meta["room_type"];
		int room_purpose = node_meta["room_purpose"];

		switch (room_shape) {
			case 1: //hexagon
				no_touchy_space[0] = room_bounding_zone[0];
				break;
			case 2:
				no_touchy_space[0] = room_bounding_zone[0];
				no_touchy_space[1] = room_bounding_zone[1];
				break;
		}
		m_Room_Vertex *new_room = new m_Room_Vertex{ i, room_shape, rnd->GetInteger(size_constraints[0], size_constraints[1]), room_purpose, static_cast<int>(m_level_point_total / Nodes.size()), room_color[0], no_touchy_space, Vector2i{ 0, 0 }, HashMap<String, m_Room_Edge *>{} };
		rooms_graph->vertices.insert(hash_name, new_room);
	}

	rooms_graph->vertices["node_0"]->location = starting_location;

	for (int i = 0; i < Edges.size(); i++) {
		Dictionary edge = Edges[i];
		String edge_dict_name = vformat("edge_%d", i);

		Dictionary edge_meta = edge[edge_dict_name];
		if (edge_meta.has("direction")) {
			Array edge_direction_meta = edge_meta["direction"];
			Array edge_distance_extents = edge_meta["distance_extents"];
			int distance_constraint = edge_meta["distance_constraint"];
			Vector2i direction{ edge_direction_meta[0], edge_direction_meta[1] };
			m_Room_Edge *new_edge = new m_Room_Edge{ rnd->GetInteger(edge_distance_extents[0], edge_distance_extents[1]), distance_constraint, direction, rooms_graph->vertices[edge_meta["to"]] };
			String edge_hash_name = vformat("%s_edge_%d", edge_meta["from"], rooms_graph->vertices[edge_meta["from"]]->edges.size());
			Vector2i from_radius{ rooms_graph->vertices[edge_meta["from"]]->radius, rooms_graph->vertices[edge_meta["from"]]->radius };
			Vector2i to_radius{ rooms_graph->vertices[edge_meta["to"]]->radius, rooms_graph->vertices[edge_meta["to"]]->radius };
			rooms_graph->vertices[edge_meta["to"]]->location = rooms_graph->vertices[edge_meta["from"]]->location + Vector2i(((new_edge->weight + rooms_graph->vertices[edge_meta["from"]]->radius + rooms_graph->vertices[edge_meta["to"]]->radius) * new_edge->direction[0] / distance_constraint), ((new_edge->weight + rooms_graph->vertices[edge_meta["from"]]->radius + rooms_graph->vertices[edge_meta["to"]]->radius) * new_edge->direction[1] / distance_constraint));
			rooms_graph->vertices[edge_meta["from"]]->edges.insert(edge_hash_name, new_edge);
		} else {
			String edge_hash_name = vformat("%s_edge_%d", edge_meta["from"], rooms_graph->vertices[edge_meta["from"]]->edges.size());
			m_Room_Edge *new_edge = new m_Room_Edge{ 0, 0, Vector2i(0, 0), rooms_graph->vertices[edge_meta["to"]] };
			rooms_graph->vertices[edge_meta["from"]]->edges.insert(edge_hash_name, new_edge);
		}
	}
	return rooms_graph;
}

/* What I can do is preprocess a map of color combinations for nodes based on a configuration file
 * Going through each non-start/-end node, if a pattern matching one in the map is found,
 * Replace the tiles in the match with the specified pattern in the map
 *
 */
void LevelGenerator::m_ReplaceNodesInPattern(m_Rooms_Graph *rooms_graph) {
}

/* Connects the room centers defined on an edge to each other with a line of tiles
 * 	FOR FUTURE: have an argument to vary the width of the connection
 *
 * Arguments:
 *   tile_bit_map: Reference to the List of tile positions and the Tile type defined for that position
 *    m_Rooms_Graph: Pointer to the starting node of the directed graph of rooms
 *
 * Returns:
 *   No Direct Returns
 *
 * Out variables:
 *    tile_bit_map: Updates the connecting tiles between rooms as ordinary tiles
 */
void LevelGenerator::m_ConnectGraphNodes(Vector<uint16_t> &tile_bit_map, m_Rooms_Graph *graph) {
	int nums_rooms = graph->vertices.size();
	for (int i = 0; i < nums_rooms; i++) {
		String node_hash = vformat("node_%d", i);
		UtilityFunctions::print("color: ", graph->vertices[node_hash]->color);
		int num_edges = graph->vertices[node_hash]->edges.size();
		for (int j = 0; j < num_edges; j++) {
			String edge_hash = vformat("node_%d_edge_%d", i, j);
			float dist = m_HexDistance(graph->vertices[node_hash]->location, graph->vertices[node_hash]->edges[edge_hash]->destination->location) / 2.0f;
			for (int i = 0; i < 5; i++) {
				int x = Math::round((graph->vertices[node_hash]->location.x - graph->vertices[node_hash]->edges[edge_hash]->destination->location.x) / dist);
				int y = Math::round((graph->vertices[node_hash]->location.y - graph->vertices[node_hash]->edges[edge_hash]->destination->location.y) / dist);
				int mult = Math::max(Math::abs(x), Math::abs(y));
				int inverse = i - mult;
				x *= inverse;
				y *= inverse;
				UtilityFunctions::print("Info: ", dist, ", (", x, ",", y, ")");
				UtilityFunctions::print("Notted: (", x ^ x, ", ", y ^ y, ")");
				UtilityFunctions::print(graph->vertices[node_hash]->edges[edge_hash]->direction);

				if (num_edges > 0) {
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location, graph->vertices[node_hash]->edges[edge_hash]->destination->location);
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location + Vector2i(x, y), graph->vertices[node_hash]->edges[edge_hash]->destination->location + Vector2i(x, y));
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location + Vector2i(x, 0), graph->vertices[node_hash]->edges[edge_hash]->destination->location + Vector2i(x, 0));
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location + Vector2i(0, y), graph->vertices[node_hash]->edges[edge_hash]->destination->location + Vector2i(0, y));
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location + Vector2i(-x, -y), graph->vertices[node_hash]->edges[edge_hash]->destination->location + Vector2i(-x, -y));
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location + Vector2i(-x, 0), graph->vertices[node_hash]->edges[edge_hash]->destination->location + Vector2i(-x, 0));
					m_DrawLineTiles(tile_bit_map, graph->vertices[node_hash]->location + Vector2i(0, -y), graph->vertices[node_hash]->edges[edge_hash]->destination->location + Vector2i(0, -y));
				}
			}
		}
	}
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
void LevelGenerator::m_GenerateRoom(Vector<uint16_t> &tile_map, HashMap<String, Ref<Tile>> *grid_of_tiles, TileGrid *root, Vector<Ref<Tile>> &spawnable_locations) {
	Object *GenerationCommunicator = Engine::get_singleton()->get_singleton("GenerationCommunicatorSingleton");
	ResourceLoader *m_rl = memnew(ResourceLoader);
	int counter = 0;
	m_rl->initialize_class();
	for (int i = 0; i < tile_map.size(); i++) {
		if (tile_map.get(i) > 0) {
			int q = i / m_maximum_grid_size[1];
			int r = i - (q * m_maximum_grid_size[1]);

			Vector3 location = TileGrid::GetPositionForHexFromCoordinate(Vector2i(q, r), m_outer_size, m_is_flat_topped);
			//
			int tile_type = tile_map.get(i);
			//
			if (!(tile_type & 0x8000)) {
				Ref<Tile> new_tile = m_InstantiateTile(GenerationCommunicator, spawnable_locations, location, q, r, tile_type);
				grid_of_tiles->insert(vformat("hex %d,%d", q, r), new_tile);

				TileCollision *m_collision_body = memnew(TileCollision);
				CollisionShape3D *m_collision_shape = memnew(CollisionShape3D);
				TileMeshGenerator *m_mesh_generator = memnew(TileMeshGenerator(m_inner_size, m_outer_size, m_height, m_is_flat_topped));

				String m_tile_mesh_name = vformat("res://Assets/Tile_Meshes/Mesh_%d_%d_%d_%d_%d.tres", (m_inner_size * 10), (m_outer_size * 10), (m_height * 10), (int)m_is_flat_topped, tile_map.get(i));
				String mesh_material_names[] = {
					"res://Assets/Materials/test_tile_material.tres",
					"res://Assets/Materials/test_interactable_tile_material.tres",
					"res://Assets/Materials/test_interactable_tile_material.tres",
					"res://Assets/Materials/test_obstacle_tile_material.tres",
					"res://Assets/Materials/test_spawner_tile_material.tres",
					"res://Assets/Materials/test_starter_tile_material.tres"
				};
				m_SetTileMeshAndMaterial(m_mesh_generator, m_rl, mesh_material_names[(tile_type & 0xFF) - 1], m_tile_mesh_name, tile_type);
				counter++;
				m_collision_body->set_name(vformat("Hex %d,%d", q, r));
				root->add_child(m_collision_body, true, Node::INTERNAL_MODE_BACK);
				m_collision_body->set_owner(root->get_owner());

				m_collision_body->add_child(m_collision_shape);
				m_collision_shape->set_owner(root->get_owner());

				m_collision_body->add_child(m_mesh_generator);
				m_mesh_generator->set_owner(root->get_owner());

				m_collision_body->set_ray_pickable(true);
				//m_mesh_generator->create_convex_collision();
				m_collision_shape->make_convex_from_siblings();
				m_collision_body->set_position(location);
			} else {
				TileMeshGenerator *m_mesh_generator = memnew(TileMeshGenerator(m_inner_size, m_outer_size, m_height, m_is_flat_topped));

				String m_tile_mesh_name = vformat("res://Assets/Tile_Meshes/Mesh_%d_%d_%d_%d_%d.tres", (m_inner_size * 10), (m_outer_size * 10), (m_height * 10), (int)m_is_flat_topped, tile_map.get(i));
				String mesh_material_names[] = {
					"res://Assets/Materials/test_tile_material.tres",
					"res://Assets/Materials/test_interactable_tile_material.tres",
					"res://Assets/Materials/test_interactable_tile_material.tres",
					"res://Assets/Materials/test_obstacle_tile_material.tres",
					"res://Assets/Materials/test_spawner_tile_material.tres",
					"res://Assets/Materials/test_starter_tile_material.tres"
				};
				m_SetTileMeshAndMaterial(m_mesh_generator, m_rl, mesh_material_names[(tile_type & 0xFF) - 1], m_tile_mesh_name, tile_type);
				m_mesh_generator->set_name(vformat("Hex %d,%d", q, r));
				root->add_child(m_mesh_generator, true, Node::INTERNAL_MODE_BACK);
				m_mesh_generator->set_owner(root->get_owner());
				m_mesh_generator->set_position(location);
			}
		}
	}
	UtilityFunctions::print("deleted m_rl");
	memdelete(m_rl);
}

/*  Tile Creation Factory Method to instantiate a Reference to a Tile Resource given a certain set of criteria
 *
 * Arguments:
 *    GenerationCommnicator: Singleton object that controls connection between low-level Tile objects in C++ and character controls in C#
 *    spawnable_locations: Reference to the list of Tile Resources that can spawn enemies
 *    location: Vector3 position of the tile within World Space
 *    q: column location of the tile; q in axial representation
 *    r: row location of the tile; r in axial represntation
 *    tile_type: int representation of the desired type of tile to be instantiated
 *
 * Returns:
 *    Ref<Tile>: Returns an instantiated reference to the Tile Resource with the provided characteristics
 *
 * Out Variables:
 *    spawnable_locations: If the instantiated tile has the ability to spawn enemies, it is added to the list of spawnable locations
 */
Ref<Tile> LevelGenerator::m_InstantiateTile(Object *GenerationCommunicator, Vector<Ref<Tile>> &spawnable_locations, Vector3 location, int q, int r, uint16_t tile_type) {
	Object *daemon = Engine::get_singleton()->get_singleton("Daemon");
	Ref<Tile> new_tile;
	tile_type &= 0x00FF;
	switch (tile_type) {
		case 1:
			new_tile = Ref<Ordinary>(memnew(Ordinary(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type)));
			break;
		case 2:
			new_tile = Ref<Interactable>(memnew(Interactable(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type, 2)));
			if (GenerationCommunicator) {
				new_tile->call("AddStepOnEvent", GenerationCommunicator->call("GetGenerateItemSignal"));
			}
			break;
		case 3:
			new_tile = Ref<Interactable>(memnew(Interactable(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type, 4)));
			if (GenerationCommunicator) {
				new_tile->call("AddStepOnEvent", GenerationCommunicator->call("GetGenerateItemSignal"));
			}
			break;
		case 4:
			new_tile = Ref<Obstacle>(memnew(Obstacle(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type)));
			break;
		case 5:
			new_tile = Ref<UnitSpawner>(memnew(UnitSpawner(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type)));
			if (daemon->has_method("AddEnemySpawnLocation")) {
				daemon->call("AddEnemySpawnLocation", new_tile);
			}
			//spawnable_locations.push_back(new_tile);
			if (GenerationCommunicator) {
				Callable signal = GenerationCommunicator->call("GetSpawnEnemySignal");
				new_tile->call("SetSpawnerCallable", signal);
			}
			break;
		case 6:
			new_tile = Ref<StartingTile>(memnew(StartingTile(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type)));
			if (daemon->has_method("AddPlayerSpawnLocation")) {
				daemon->call("AddPlayerSpawnLocation", new_tile);
			}
			if (GenerationCommunicator) {
				Callable signal = GenerationCommunicator->call("GetSpawnCharacterSignal");
				new_tile->call("SetSpawnerCallable", signal);
			}
			break;
		default:
			new_tile = Ref<Ordinary>(memnew(Ordinary(location, q, r, m_is_flat_topped, m_outer_size, m_inner_size, m_height, tile_type)));
			break;
	}
	return new_tile;
}

/* For a given tile, set the mesh and material
 *
 * Arguments:
 *    mesh_generator: Pointer to the Mesh Generator object that either loads the saved mesh or creates the new mesh type if it is not saved
 *    ResourceLoader: Pointer to the ResourceLoader Resource that loads a saved mesh
 *    mesh_material_name: String containing the absoluate path to the Material for the given mesh
 *    tile_mesh_name: String containing the absolute path to the Tile Mesh for the given tile
 *    tile_type: int representation of the type of tile
 *
 * Returns:
 *    No Direct Returns
 */
void LevelGenerator::m_SetTileMeshAndMaterial(TileMeshGenerator *mesh_generator, ResourceLoader *rl, String mesh_material_name, String tile_mesh_name, uint16_t tile_type) {
	Ref<Mesh> m_mesh;
	Ref<ShaderMaterial> m_mesh_material;
	//if (rl->exists(tile_mesh_name)) {
	//	m_mesh = rl->load(tile_mesh_name, "Mesh");
	//mesh_generator->set_mesh(m_mesh);
	//} else {
	m_mesh = mesh_generator->DrawMesh(tile_type);
	ResourceSaver *m_rs = memnew(ResourceSaver);
	m_rs->save(m_mesh, tile_mesh_name, ResourceSaver::FLAG_COMPRESS);
	memdelete(m_rs);
	//}
	if (rl->exists(mesh_material_name)) {
		m_mesh_material = rl->load(mesh_material_name)->duplicate();
		int surface_count = m_mesh->get_surface_count();
		for (int i = 0; i < surface_count; i++) {
			m_mesh->surface_set_material(i, m_mesh_material);
			m_mesh_material->set_local_to_scene(true);
		}
		m_mesh->set_local_to_scene(true);
	}
}

LevelGenerator::m_Best_Neighbors LevelGenerator::m_FindNearest(m_Room_Tree_Node *node, Vector2i goal_room, m_Best_Neighbors best_neighbors, int level) {
	if (node == nullptr) {
		return best_neighbors;
	}
	int curr_distance = m_HexDistance(node->room_center, goal_room);
	int best_distance = m_HexDistance(goal_room, best_neighbors.neighbor_list[0]);
	int second_best_distance = m_HexDistance(goal_room, best_neighbors.neighbor_list[1]);

	m_Room_Tree_Node *good_side = nullptr;
	m_Room_Tree_Node *bad_side = nullptr;

	if (node->room_center != goal_room) {
		if (curr_distance < second_best_distance) {
			if (curr_distance < best_distance) {
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
		if (theoretical_distance <= second_best_distance) {
			best_neighbors = m_FindNearest(bad_side, goal_room, best_neighbors, level++);
		}
	} else {
		Vector2i theoretical_point = Vector2i{ goal_room[0], node->room_center[1] };
		int theoretical_distance = m_HexDistance(goal_room, theoretical_point);
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
 * m_num_rooms_remaining: Reference to the # of rooms left available to generate
 * current_level: Describes the level of the tree the generator is currently on
 * max_level: Describes the max level of the tree that the generator can go to
 * max_grid_size: 2D Integer Vector that describes the maximum row, column boundaries of the generator
 *
 * Returns:
 * tile_bit_map: Filled with all locations of tiles
 * root_room: Pointer to the root_room of the k-d tree spanning all generated rooms
 */
LevelGenerator::m_Room_Tree_Node *LevelGenerator::m_GenerateTileBitMap(Vector<uint16_t> &tile_bit_map, m_Room_Tree_Node *root_room, int &num_of_rooms_remaining, int current_level, int max_level, Vector2i max_grid_size) {
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
		root_room = new m_Room_Tree_Node{ new_room, nullptr, nullptr };
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

bool LevelGenerator::m_HasNeighbors(Vector<uint16_t> &tile_map, int q, int r) {
	bool neighbor_exists =
			tile_map.get(q * m_maximum_grid_size[1] + r + 1) > 0 ||
			tile_map.get((q + 1) * m_maximum_grid_size[1] + r) > 0 ||
			tile_map.get((q + 1) * m_maximum_grid_size[1] + r - 1) > 0 ||
			tile_map.get(q * m_maximum_grid_size[1] + r - 1) > 0 ||
			tile_map.get((q - 1) * m_maximum_grid_size[1] + r) > 0 ||
			tile_map.get((q - 1) * m_maximum_grid_size[1] + r + 1) > 0;
	return neighbor_exists;
}

/* For a given graph of room nodes, fill the tile bit map with the corresponding tiles and types in the rooms
 *
 * Arguments:
 *    tile_bit_map: Reference to the vector of tile positions and the corresponding type
 *    graph: Pointer to the starting node of the graph of rooms
 *    grid_origin: Vector2i location of the tile situated at the origin of the grid in axial notation
 *
 * Returns:
 *    No Direct Returns
 *
 * Out Variables:
 *    tile_bit_map: Tile positions corresponding to the locations within rooms in the graph are filled out with their tile types
 */
void LevelGenerator::m_GenerateGraphTileBitMap(Vector<uint16_t> &tile_bit_map, m_Rooms_Graph *graph, Vector2i grid_origin) {
	SeededRandomAccess *rnd = SeededRandomAccess::GetInstance();
	int num_of_rooms = graph->vertices.size();
	for (int i = 0; i < num_of_rooms; i++) {
		String node_hash = vformat("node_%d", i);
		Vector2i room_location = graph->vertices[node_hash]->location;
		int room_shape = graph->vertices[node_hash]->room_shape;
		int interactable_points = graph->vertices[node_hash]->interactable_points;
		int q, r;
		//UtilityFunctions::print("Current Room Type: ", graph->vertices[node_hash]->room_shape);
		//UtilityFunctions::print("Current Room Purpose: ", graph->vertices[node_hash]->purpose);
		//UtilityFunctions::print(vformat("Room location: %d, %d", room_location[0], room_location[1]));
		Vector<int> interactables = m_GenerateInteractableType(interactable_points);
		int radius = graph->vertices[node_hash]->radius;
		switch (room_shape) {
				//Pure Hexagon Room
			case 1:
				for (int q = -radius; q <= radius; q++) {
					int r1 = Math::max(-radius, -q - radius);
					int r2 = Math::min(radius, -q + radius);
					for (int r = r1; r <= r2; r++) {
						if (
								((Math::abs(q) == radius / 4) && (Math::abs(r) == radius / 4)) ||
								((Math::abs(q) == radius / 2) && (Math::abs(-q - r) == Math::round(radius / 3.0f))) ||
								((Math::abs(-q - r) == Math::round(radius / 3.0f)) && Math::abs(r) == radius / 2)) {
							/*if (Math::abs(q) == radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0005);
								tile_bit_map.set((room_location[0] + q - 3) * m_maximum_grid_size[1] + (room_location[1] + r), 0xB100);
							} else if (Math::abs(q) == -radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0005);
								tile_bit_map.set((room_location[0] + q + 3) * m_maximum_grid_size[1] + (room_location[1] + r), 0x8200);
							} else if (Math::abs(r) == radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x8405);
							} else if (Math::abs(r) == -radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x8805);
							} else {*/
							tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0005);
							//}
						} else {
							if (q == radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001);
								if (tile_bit_map.get((room_location[0] + q + 1) * m_maximum_grid_size[1] + room_location[1] + r) == 0 ||
										tile_bit_map.get((room_location[0] + q) * m_maximum_grid_size[1] + room_location[1] + r - 2) == 0 ||
										tile_bit_map.get((room_location[0] + q) * m_maximum_grid_size[1] + room_location[1] + r - 1) == 0) {
									tile_bit_map.set((room_location[0] + q + 1) * m_maximum_grid_size[1] + (room_location[1] + r), 0xA100);
								}
							} else if (q == -radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001);
								tile_bit_map.set((room_location[0] + q - 1) * m_maximum_grid_size[1] + (room_location[1] + r), 0x8C01);
							} else if (r == radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001);
								tile_bit_map.set((room_location[0] + q - 1) * m_maximum_grid_size[1] + (room_location[1] + r + 1), 0x8601);
							} else if (r == -radius) {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001);
								tile_bit_map.set((room_location[0] + q + 1) * m_maximum_grid_size[1] + (room_location[1] + r - 1), 0xB001);
							} else {
								tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001);
							}
						}
					}
				}
				//				q = rnd->GetInteger(-radius, radius);
				//				r = rnd->GetInteger(Math::max(-radius, -q - radius), Math::min(radius, -q + radius));
				//				while ((tile_bit_map.get((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r)) > 1)) {
				//					q = rnd->GetInteger(-radius, radius);
				//					r = rnd->GetInteger(Math::max(-radius, -q - radius), Math::min(radius, -q + radius));
				//}

				for (int i = 0; i < interactables.size(); i++) {
					q = rnd->GetInteger(-radius, radius);
					r = rnd->GetInteger(Math::max(-radius, -q - radius), Math::min(radius, -q + radius));
					while ((tile_bit_map.get((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r)) > 1)) {
						q = rnd->GetInteger(-radius, radius);
						r = rnd->GetInteger(Math::max(-radius, -q - radius), Math::min(radius, -q + radius));
					}
					if (q == radius) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001 + interactables[i]);
					} else if (q == -radius) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001 + interactables[i]);
					} else if (r == radius) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001 + interactables[i]);
					} else if (r == -radius) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001 + interactables[i]);
					} else {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 0x0001 + interactables[i]);
					}
				}

				break;
			//Vertical Ellipse
			case 2:
				for (int q = -radius; q <= radius; q++) {
					int r1 = Math::max(-(radius * 2), -q - (radius * 2));
					int r2 = Math::min((radius * 2), -q + (radius * 2));
					for (int r = r1; r <= r2; r++) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 1);
					}
				}
				break;
			//Parallelogram
			case 3:
				for (int q = -(radius * 2); q <= (radius * 2); q++) {
					int r1 = Math::max(-radius, -q - radius);
					int r2 = Math::min(radius, -q + radius);
					for (int r = r1; r <= r2; r++) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 1);
					}
				}
				break;
				//Rectangle
			case 4:
				for (int q = -radius; q <= radius; q++) {
					int r1 = Math::max(-radius, (-q * 2) - radius);
					int r2 = Math::min(radius, (-q * 2) + radius);
					for (int r = r1; r <= r2; r++) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 1);
					}
				}
				break;
			case 5:
				for (int q = -radius; q <= radius; q++) {
					int r1 = Math::max(-radius, (-q * 2) - radius);
					int r2 = Math::min(radius, (-q * 2) + radius);
					for (int r = r1; r <= r2; r++) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 1);
					}
				}
				break;
			case 6:
				for (int r = -radius; r <= radius; r++) {
					int q1 = Math::max(-(radius * 2), -r - (radius * 2));
					int q2 = Math::min((radius * 2), -r + (radius * 2));
					for (int q = q1; q <= q2; q++) {
						tile_bit_map.set((room_location[0] + q) * m_maximum_grid_size[1] + (room_location[1] + r), 1);
					}
				}
				break;
		}
		if (i == 0) {
			tile_bit_map.set(((room_location[0] + (radius / 3)) * m_maximum_grid_size[1] + (room_location[1])), 6);
			tile_bit_map.set(((room_location[0] - (radius / 3)) * m_maximum_grid_size[1] + (room_location[1] + (radius / 3))), 6);
			tile_bit_map.set(((room_location[0]) * m_maximum_grid_size[1] + (room_location[1] + (radius / 3))), 6);
		}
	}
}

/* Generates the interactable types for a room given a certain point allocation
 *
 * Arguments:
 *    num_points: Point allocation for the total sum of interactables to be spawned
 *
 * Returns:
 *     Vector<int>: Vector containing the int representation of interactables to be placed within a room
 */
Vector<int> LevelGenerator::m_GenerateInteractableType(int num_points) {
	Vector<int> generated_interactables{};
	SeededRandomAccess *rnd = SeededRandomAccess::GetInstance();
	while (num_points > 0) {
		int which_layout = rnd->GetInteger(1, 2);
		switch (which_layout) {
			case 1:
				generated_interactables.push_back(which_layout);
				num_points -= 2;
				break;
			case 2:
				generated_interactables.push_back(which_layout);
				num_points -= 4;
				break;
		}
	}
	return generated_interactables;
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
bool LevelGenerator::m_OverlappingRooms(const Vector<uint16_t> &tile_bit_map, Vector2i center, int radius) {
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
void LevelGenerator::m_FillBitMap(Vector<uint16_t> &tile_bit_map, int q_center, int r_center, int radius) {
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
void LevelGenerator::m_ConnectTiles(Vector<uint16_t> &tile_bit_map, Vector<Vector2i> room_neighbors) {
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
void LevelGenerator::m_DrawLineTiles(Vector<uint16_t> &tile_bit_map, Vector2i first_room_center, Vector2i second_room_center) {
	int distance = m_HexDistance(first_room_center, second_room_center);
	for (int i = 0; i <= distance; i++) {
		Vector2i location = m_HexRound(first_room_center, second_room_center, distance, i);
		if (location[0] >= 0 && location[1] >= 0) {
			tile_bit_map.set(location[0] * m_maximum_grid_size[1] + location[1], 0x0001);
			//tile_bit_map.set(location[0] * _maximum_grid_size[1] + location[1] + 1, 0x0001);
		}
	}
}

Vector2i LevelGenerator::m_HexRound(Vector2i first_room, Vector2i second_room, int distance, int step) {
	Vector2 approximate_location = Vector2(first_room[0] + (second_room[0] - first_room[0]) * (1.0f / distance) * step, first_room[1] + (second_room[1] - first_room[1]) * (1.0f / distance) * step);
	//	int q = (Math::round(approximate_location[0]));
	//	int r = (Math::round(approximate_location[1]));
	//
	//	float q_diff = Math::abs(q - approximate_location[0]);
	//	float r_diff = Math::abs(r - approximate_location[1]);
	//
	//	if (Math::abs(q) >= Math::abs(r)) {
	//		return Vector2i(q + (Math::round(q_diff + 0.5f * r_diff)), r);
	//	} else {
	//		return Vector2i(q, r + (Math::round(r_diff + 0.5f * q_diff)));
	//	}
	float qgrid = Math::round(approximate_location[0]);
	float rgrid = Math::round(approximate_location[1]);

	approximate_location[0] -= qgrid;
	approximate_location[1] -= rgrid;

	float dq = 0;
	float dr = 0;

	if (Math::abs(approximate_location[0]) >= Math::abs(approximate_location[1])) {
		dq = Math::round(approximate_location[0] + 0.5f * approximate_location[1]);
	} else {
		dr = Math::round(approximate_location[1] + 0.5f * approximate_location[0]);
	}
	return Vector2i(Math::round(qgrid + dq), Math::round(rgrid + dr));
}

int LevelGenerator::m_HexDistance(Vector2i first_room, Vector2i second_room) {
	int distance = Math::round((Math::abs(first_room[0] - second_room[0]) +
									   Math::abs(first_room[0] + first_room[1] -
											   second_room[0] - second_room[1]) +
									   Math::abs(first_room[1] - second_room[1])) /
			2.0f);
	return distance;
}

int LevelGenerator::GetNumRooms() {
	return m_num_rooms;
}
