using Godot;
using System.Collections.Generic;

public partial class MoveToLocation: Node3D {
    private Vector3 current_location, wanted_location, hit_location;
    private bool moving = false;
    private Queue<Tile> path = new Queue<Tile>();

    [Export]
    public float movement_time = 1.0f;

    [Export]
    public float turning_time = .3f;

    private Vector3I[] neighbor_matrix = {new Vector3I(-1,0,0), new Vector3I(1,0,0), new Vector3I(0,1,0), new Vector3I(1,1,0), new Vector3I(0,-1,0), new Vector3I(1,-1,0)};

    public void MoveUnit(Vector3 location) {
      if(path.Count <= 0 && !moving) {
        current_location = this.Position;
      }
    }

    private void CalculatePath(Vector3 starting_location, Vector3 end_location) {
      Tile first_node, wanted_node;
      List<Tile> open_tiles = new List<Tile>();
      HashSet<Tile> closed_tiles = new HashSet<Tile>();

      first_node = TileGrid.findTileOnGrid(TileGrid.positionToGrid(starting_location));
      wanted_node = TileGrid.findTileOnGrid(TileGrid.positionToGrid(end_location));

      open_tiles.Add(first_node);

      while(open_tiles.Count > 0) {
        Tile tile = open_tiles[0];
        for(int i = 1; i < open_tiles.Count; i++) {
          if(open_tiles[i].f_cost <= tile.f_cost) {
            if(open_tiles[i].h_cost < tile.h_cost) {
              tile = open_tiles[i];
            }
          }
        }
        open_tiles.Remove(tile);
        closed_tiles.Add(tile);
        if(tile == wanted_node) {
          RetracePath(first_node, wanted_node);
          return;
        }

        foreach(Tile neighbor in TileGrid.GetNeighbors(tile)) {
          if(neighbor.obstacle || closed_tiles.Contains(neighbor)) {
            continue;
          }

          int new_cost_to_neighbor = tile.g_cost + CalculateDistance(tile, neighbor);
          if(new_cost_to_neighbor < neighbor.g_cost || !open_tiles.Contains(neighbor)) {
            neighbor.g_cost = new_cost_to_neighbor;
            neighbor.h_cost = CalculateDistance(neighbor, wanted_node);
            neighbor.parent = tile;

            if(!open_tiles.Contains(neighbor)) {
              open_tiles.Add(neighbor);
            }
          }
        }
      }
    }

    private void RetracePath(Tile start_tile, Tile end_tile) {
      List<Tile> retraced_path = new List<Tile>();
      Tile current_tile = end_tile;

      while(current_tile != start_tile) {
        retraced_path.Add(current_tile);
        current_tile = current_tile.parent;
      }
      retraced_path.Reverse();
      path = new Queue<Tile>(retraced_path);
    }

    private int CalculateDistance(Tile location, Tile destination) {
      Vector2 first_tile = new Vector2(location.column, location.row);
      Vector2 last_tile = new Vector2(destination.column, destination.row);

      int distance = DistanceHex(first_tile, last_tile);
      return distance;
    }


}
