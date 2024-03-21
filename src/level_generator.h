#ifndef LEVEL_GENERATOR_H
#define LEVEL_GENERATOR_H

#include <cstdint>
#include <godot_cpp/classes/node.hpp>
#include <godot_cpp/classes/node3d.hpp>
#include <godot_cpp/variant/vector2i.hpp>
#include <godot_cpp/templates/hash_map.hpp>
#include <godot_cpp/templates/list.hpp>
#include <godot_cpp/templates/vector.hpp>

#include <tile.h>
#include <tilegrid.h>

namespace godot {

class LevelGenerator : public Object {
	GDCLASS(LevelGenerator, Object);

private:
	float _outerSize;
	float _innerSize;
	float _height;
	bool _is_flat_topped;
    Vector2i _maximum_grid_size;

protected:
	static void _bind_methods();

public:
	LevelGenerator();
    LevelGenerator(const Vector2i &grid_size);
    LevelGenerator(float outer_size, float inner_size, float height, bool is_flat_topped,const Vector2i &grid_size);
	~LevelGenerator();

    HashMap<String, Tile *> generateLevel(TileGrid *root);

private:
    void _generateRoom(Vector<uint8_t> &tile_map, HashMap<String, Tile *> grid_of_tiles, TileGrid *root);
    Vector<Vector2i> _generateMST(Vector<Vector2i> room_centers);
    void _generateTileBitMap(Vector<uint8_t> &tile_bit_map, Vector<Vector2i> room_centers, int &num_of_rooms_remaining,
                             int current_level, int max_level, Vector2i max_grid_size);
    bool _overlappingRooms(Vector<uint8_t> tile_bit_map, Vector2i center, int radius);
    void _fillBitMap(Vector<uint8_t> &tile_bit_map, int q_center, int r_center, int radius);
    void _connectTiles(Vector<uint8_t> tile_bit_map, Vector<Vector2i> room_neighbors);
    void _drawLineTiles(Vector<uint8_t> tile_bit_map, Vector2i first_room_center, Vector2i second_room_center);
    Vector2i _hexRound(Vector2i first_room, Vector2i second_room, int distance, int step);

};
} //namespace godot
#endif
