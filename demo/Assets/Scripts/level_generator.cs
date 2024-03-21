using Godot;
using System;
using System.Collections.Generic;

public partial class level_generator
{
    private float outerSize;
    private float innerSize;
    private float height;
    private bool isFlatTopped;
    private Material material;

    //X is the # of rows
    //Y is the # of columns
    private Vector2I maximum_grid_size;

    //Default Constructor:
    //Set the tile attributes to their defaults
    public level_generator() {
        outerSize = 1.0f;
        innerSize = 0.0f;
        height = 1.0f;
        isFlatTopped = true;
        maximum_grid_size = new Vector2I(1000,1000);
    }
    public level_generator(Vector2I grid_size) {
        maximum_grid_size = grid_size;
    }

    public level_generator(float outerSize, float innerSize, float height, bool is_flat_topped, Vector2I grid_size) {
        this.outerSize = outerSize;
        this.innerSize = innerSize;
        this.height = height;
        this.isFlatTopped = is_flat_topped;
        this.maximum_grid_size = grid_size;
    }

    public Dictionary<string, Tile> generateLevel(TileGrid root) {
        Dictionary<string, Tile> tile_grid = new Dictionary<string, Tile>();
        List<Vector2I> room_centers = new List<Vector2I>(0);
        byte[] tile_bit_map = new byte[maximum_grid_size[0] * maximum_grid_size[1]];
        int num_of_rooms = SeededRandomAccess.staticSeededRandomAccess.getRandomInteger(20,30);
        GD.Print($"number of rooms: {num_of_rooms}");
        GD.Print(tile_bit_map.Length);
        Vector2I gridCenter = new Vector2I(maximum_grid_size[0] / 2, maximum_grid_size[1] / 2);

        generateTileBitMap(tile_bit_map, room_centers, num_of_rooms, 0, 3, gridCenter);
        //List<Vector2I> room_neighbors = GenerateMST(room_centers);
        //connectTiles(tile_bit_map, room_neighbors);
        generateRoom(tile_bit_map, tile_grid, root);

        GD.Print(room_centers.Count);

        return tile_grid;

    }
    private void generateRoom(byte[] tile_map, Dictionary<string, Tile> grid_of_tiles, Node3D root) {
        for(int i = 0; i < tile_map.Length; i++) {
            if(tile_map[i] > 0) {
                int q = i / maximum_grid_size[1];
                int r = i - (q * maximum_grid_size[1]);
                Tile tile = new Tile(TileGrid.GetPositionForHexFromCoordinate(new Vector2I(q, r), outerSize, isFlatTopped), q, r, false, innerSize, outerSize, height, isFlatTopped);
                grid_of_tiles.Add($"Hex {q},{r}", tile);
                root.AddChild(tile);
                //GD.Print($"Added child {q},{r}");
            }
        }
    }

    private List<Vector2I> GenerateMST(List<Vector2I> room_centers) {
        List<Vector2I> neighbor_list = new List<Vector2I>();

        for(int i = 0; i < room_centers.Count - 2; i++) {
            int lowest_distance = 10000;
            int second_lowest_distance = lowest_distance;
            int neighbor = i;
            int second_neighbor = neighbor;
            for(int j = i + 1; j < room_centers.Count - 1; j++) {
                int distance = (Math.Abs(room_centers[i][0] - room_centers[j][0])
                                + Math.Abs(room_centers[i][0] + room_centers[i][1] - room_centers[j][0] - room_centers[j][1])
                                + Math.Abs(room_centers[i][1] - room_centers[j][1])) / 2;
                if(distance < second_lowest_distance) {
                    if(distance < lowest_distance) {
                        lowest_distance = distance;
                        neighbor = j;
                    } else {
                        second_lowest_distance = distance;
                        second_neighbor = j;
                    }
                }
            }
            GD.Print($"{i}, {neighbor}, {second_neighbor}");
            neighbor_list.Add(room_centers[i]);
            neighbor_list.Add(room_centers[neighbor]);
            neighbor_list.Add(room_centers[second_neighbor]);
        }
        return neighbor_list;
    }

    /*Generates a bit map of all tiles | The value of the 8 bit string determines the type of tile
     *Initially generates the starting shape at the pure center of grid
     *Generates a random number of rooms to generate coming out from that center room
     *The initial layer has a random # of rooms generated going at different directions | this is subtracted from the total
     *
     */
    private void generateTileBitMap(byte[] tile_bit_map, List<Vector2I> room_centers,
            int num_of_rooms_remaining, int current_level, int max_level, Vector2I max_grid_size) {
        SeededRandomAccess rnd = SeededRandomAccess.staticSeededRandomAccess;
        int gridCenterQ = max_grid_size[0];
        int gridCenterR = max_grid_size[1];
        int random_layer_rooms = Mathf.RoundToInt(rnd.getRandomInteger(5,8) * (1.0f/(current_level + 1)));
        GD.Print($"Rooms Remaining: {num_of_rooms_remaining} | Random: {random_layer_rooms}");
        int remaining_room_allocation = num_of_rooms_remaining - random_layer_rooms;
        GD.Print($"Room Allocation {remaining_room_allocation} on level {current_level}");
        int radius, q_direction, r_direction, q_offset, r_offset;
        do {
            radius = rnd.getRandomInteger(5, 8);
            q_direction = ((rnd.getRandomWholeNumber(52) % 2) * 2) - 1;
            r_direction = ((rnd.getRandomWholeNumber(52) % 2) * 2) - 1;
            q_offset = rnd.getRandomInteger(5,8);
            r_offset = rnd.getRandomInteger(5,8);
            gridCenterQ = gridCenterQ + q_direction * (5 + radius + q_offset);
            gridCenterR = gridCenterR + r_direction * (5 + radius + r_offset);
        } while (gridCenterQ < 0 ||
                gridCenterQ > maximum_grid_size[0] ||
                gridCenterR < 0 ||
                gridCenterR > maximum_grid_size[1] ||
                overlappingRooms(tile_bit_map, new Vector2I(gridCenterQ, gridCenterR), radius));
        room_centers.Add(new Vector2I(gridCenterQ, gridCenterR));
        fillBitMap(tile_bit_map, gridCenterQ, gridCenterR, radius);
        if(current_level < max_level && num_of_rooms_remaining > 0) {
            if(num_of_rooms_remaining < remaining_room_allocation) {
                generateTileBitMap(tile_bit_map, room_centers, num_of_rooms_remaining--, current_level+ 1, max_level, new Vector2I(gridCenterQ, gridCenterR));
            } else {
                for(int i = 0; i < random_layer_rooms; i++) {
                    generateTileBitMap(tile_bit_map, room_centers, remaining_room_allocation, current_level + 1, max_level, new Vector2I(gridCenterQ, gridCenterR));
                }
            }
        }

    }

    private bool overlappingRooms(byte[] tile_bit_map, Vector2I center, int radius) {
        Vector2I[] directions = {new Vector2I(1, 0), new Vector2I(1, -1), new Vector2I(0, -1),
                                 new Vector2I(-1, 0), new Vector2I(-1, 1), new Vector2I(0, 1)};
        Vector2I curr_hex = new Vector2I(center[0] - radius, center[1] + radius);
        for(int i = 0; i < 6; i++) {
            for(int j = 0; j < radius; j++) {
                if(tile_bit_map[curr_hex[0] * maximum_grid_size[1] + curr_hex[1]] > 0) {
                    return true;
                }
                curr_hex += directions[i];
            }
        }
        return false;
    }


    private void fillBitMap(byte[] tile_bit_map, int q_center, int r_center, int radius) {
        for (int q = -radius; q <= radius; q++) {
            int r1 = Mathf.Max(-radius, -q - radius);
            int r2 = Mathf.Min(radius,  -q + radius);
            for (int r = r1; r <= r2; r++) {
                tile_bit_map[(q_center + q) * maximum_grid_size[1] + (r_center + r)] = 0x0001;
            }
        }
    }

    private void connectTiles(byte[] tile_bit_map, List<Vector2I> room_neighbors) {
        for(int i = 0; i < room_neighbors.Count - 3; i+=3) {
            drawLineofTiles(tile_bit_map, room_neighbors[i], room_neighbors[i+1]);
            drawLineofTiles(tile_bit_map, room_neighbors[i], room_neighbors[i+2]);
        }
    }

    private void drawLineofTiles(byte[] tile_bit_map, Vector2I first_room_center, Vector2I second_room_center) {
        int distance = (Math.Abs(first_room_center[0] - second_room_center[0]) +
                Math.Abs(first_room_center[0] + first_room_center[1] -
                    second_room_center[0] - second_room_center[1]) +
                Math.Abs(first_room_center[1] - second_room_center[1])) / 2;
        for (int i = 0; i <= distance; i++) {
            Vector2I location = hexRound(first_room_center, second_room_center, distance, i);
            tile_bit_map[location[0] * maximum_grid_size[1] + location[1]] = 0x0001;
        }
    }

    private Vector2I hexRound(Vector2I first_room, Vector2I second_room, int distance, int step) {
        Vector2 aproximate_location = new Vector2(first_room[0] +
                (second_room[0] - first_room[0]) * (1.0f / distance * step),
                first_room[1] + (second_room[1] - first_room[1]) * (1.0f / distance * step));
        int q = Mathf.RoundToInt(aproximate_location[0]);
        int r = Mathf.RoundToInt(aproximate_location[1]);

        float q_diff = q - aproximate_location[0];
        float r_diff = r - aproximate_location[1];
        if(Mathf.Abs(q) >= Mathf.Abs(r)) {
            return new Vector2I(q + Mathf.RoundToInt(q_diff + 0.5f* r_diff), r);
        } else {
            return new Vector2I(q, r + Mathf.RoundToInt(r_diff * 0.5f*q_diff));
        }

    }

}
