using Godot;
using System.Collections.Generic;

public partial class MoveToLocation {
    private Vector3 current_location, wanted_location, hit_location;
    private bool moving = false;
    private Queue<Tile> path = new Queue<Tile>();

    [Export]
    public float movement_time = 1.0f;

    [Export]
    public float turning_time = .3f;

    private Vector3I[] neighbor_matrix = {new Vector3I(-1,0,0), new Vector3I(1,0,0), new Vector3I(0,1,0), new Vector3I(1,1,0), new Vector3I(0,-1,0), new Vector3I(1,-1,0)};

    public void MoveUnit(Vector3 location){}


}
