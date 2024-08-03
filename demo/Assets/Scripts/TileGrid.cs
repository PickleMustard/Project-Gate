using Godot;
using System.Collections.Generic;

/*public partial class TileGrid : Node3D {
    public Dictionary<string, Tile> gridOfTiles;
    //public static List<Tile> gridOfTiles;
    //For a hexagonal grid, just uses the x component of gridSize
    [Export]
    public Vector2I gridSize;
    public List<Tile> path;
    [Export]
    public float size = 3f;
    [Export]
    public int radius = 7;

    public float outerSize = 1.0f;
    public float innerSize = 0.0f;
    public float height = 1.0f;
    public bool isFlatTopped;
    public Material material;


    public override void _Ready() {
        isFlatTopped = true;
        gridOfTiles = new Dictionary<string, Tile>();
        level_generator showrooms = new level_generator(outerSize, innerSize, height, isFlatTopped, new Vector2I(1000,1000));
        gridOfTiles = showrooms.generateLevel(this);
    }

    private void LayoutGrid() {
        if(this.GetChildCount() > 0) {
            GD.Print(this.GetChildCount());
            GD.Print(GetChildCount());
            GD.Print("Deleting Children");
            foreach(Node child in this.GetChildren()) {
                child.QueueFree();
            }
        }

        GD.Print(gridSize[0]);
        GD.Print(gridSize[1]);

        /*
        for(int y = 0; y < gridSize[1]; y++) {
            for(int x = 0; x < gridSize[0]; x++) {
                Tile tile = new Tile(GetPositionForHexFromCoordinate(new Vector2I(x, y)), y, x, false, innerSize, outerSize, height, true);
                GD.Print(tile);
                this.AddChild(tile);
                gridOfTiles.Add($"Hex {y},{x}",tile);
                GD.Print($"Added child {y},{x}");
            }
        }*

        for (int q = -radius; q <= radius; q++) {
            int r1 = Mathf.Max(-radius, -q - radius);
            int r2 = Mathf.Min(radius, -q + radius);
            for(int r = r1; r <= r2; r++) {
                //Tile tile = new Tile(GetPositionForHexFromCoordinate(new Vector2I(q, r)), q, r, false, innerSize, outerSize, height, true);
                //gridOfTiles.Add($"Hex {q},{r}", tile);
                //this.AddChild(tile);
                GD.Print($"Added child {q},{r}");
            }
        }

        GD.Print(GetChildCount());
    }

    public static Vector3 GetPositionForHexFromCoordinate(Vector2I coordinate, float size, bool is_flat_topped) {
        float root3 = Mathf.Sqrt(3);
        int column = coordinate[0];
        int row = coordinate[1];
        float width;
        float height;
        float xPosition;
        float yPosition;
        bool shouldOffset;
        float horizontalDistance;
        float verticalDistance;
        if(!is_flat_topped){
            shouldOffset = (row %2) == 0;
            width = root3 * size;
            height = 2f * size;

            horizontalDistance = width;
            verticalDistance = height * (3f/4f);

            xPosition = (root3 *coordinate[0] + root3/2.0f * coordinate[1]);
            yPosition = (3.0f/2.0f * coordinate[1]);
        } else {
            shouldOffset = (column %2) == 0;
            width = 2f * size;
            height = root3 * size;

            horizontalDistance = width* (3f/4f);
            verticalDistance = height;

            xPosition = (3.0f/2.0f *coordinate[0]) * size;
            yPosition = (root3/2.0f * coordinate[0] + root3 * coordinate[1]) * size;
        }

        return new Vector3(xPosition, 0, yPosition);
    }


    public Vector2 positionToGrid(Vector3 location) {
        float easySqrt = (size * Mathf.Sqrt(3));
        float q = ((2f/3f)*location[0]) / 3f;
        float r = ((-1f/3f)*location[0] + (Mathf.Sqrt(3)/3f) * (location[2] + (easySqrt/2f))) / 3f;
        //Debug.Log(string.Format("Location within Grid is: {0} | Q and R: {1},{2}",location, q, r));
        return axialRound(new Vector2(q, r));
    }

    public Tile findTileOnGrid(Vector2I location){
        Tile foundTile = gridOfTiles[$"Hex {location[0]},{location[1]}"];
        return foundTile;
    }

    /*private void OnDrawGizmos() {
        Gizmos.DrawWireCube(new Vector3(0,0,0), new Vector3(gridSize.x * 3, 1, gridSize.y * 3));
        if(gridOfTiles.Count > 0){
            foreach(Tile t in gridOfTiles.Values){
                Gizmos.color = (t.obstacle)?Color.white:Color.red;
                if(path != null){
                    if(path.Contains(t))
                        Gizmos.color = Color.blue;
                }
                Gizmos.DrawCube(t.tilePosition, Vector3.one);
            }
        }
    }*

    //Get the neighbors of the Tile (X is the column and Z is the row)
    public List<Tile> GetNeighbors(Tile tile){
        List<Tile> neighbors = new List<Tile>();
        string[] locations = new string[6];
        locations[0] = $"Hex {tile.column},{tile.row+1}"; //Top Neighbor
        locations[1] = $"Hex {tile.column+1},{tile.row}"; //Top Right Neighbor
        locations[2] = $"Hex {tile.column+1},{tile.row-1}"; //Bottom Right Neighbor
        locations[3] = $"Hex {tile.column},{tile.row-1}"; //Bottom Neighbor
        locations[4] = $"Hex {tile.column-1},{tile.row}"; //Bottom Left Neighbor
        locations[5] = $"Hex {tile.column-1},{tile.row+1}"; //Top Left Neighbor
        for(int i = 0; i < locations.Length; i++){
            if(gridOfTiles.ContainsKey(locations[i])){
                GD.Print(string.Format("Neighbors of {0} : N{1}, {2}", tile, i, gridOfTiles[locations[i]]));
                neighbors.Add(gridOfTiles[locations[i]]);
            }
        }
        GD.Print(string.Format("Number of Neighbors: {0}", neighbors.Count));
        return neighbors;
    }


    /*
    For Reference:
        Q is referenced as x
        R is referenced as y
        S is referenced as z
    *
    static Vector2 axialRound(Vector2 hex){
        return cubeToAxial(cubeRound(axialToCube(hex)));
    }

    static Vector3 cubeRound(Vector3 hex){
        //double roundedQ = Math.Round(hex.x, MidpointRounding.AwayFromZero);
        //double roundedR = Math.Round(hex.y, MidpointRounding.AwayFromZero);
        //double roundedS = Math.Round(hex.z, MidpointRounding.AwayFromZero);
        float roundedQ = Mathf.Round(hex[0]);
        float roundedR = Mathf.Round(hex[1]);
        float roundedS = Mathf.Round(hex[2]);

        float qDiff = Mathf.Abs(roundedQ - hex[0]);
        float rDiff = Mathf.Abs(roundedR - hex[1]);
        float sDiff = Mathf.Abs(roundedS - hex[2]);

        GD.Print(string.Format("Rounded Numbers: ({0},{1},{2}) | Their Differences: {3},{4},{5}", roundedQ, roundedR, roundedS, qDiff, rDiff, sDiff));

        if (qDiff > rDiff && qDiff > sDiff) {
            roundedQ = -roundedR-roundedS;
        } else if (rDiff >= sDiff){
            roundedR = -roundedQ-roundedS;
        } else {
            roundedS = -roundedQ-roundedR;
        }

        GD.Print(string.Format("Final Cube Coords: ({0},{1},{2})", roundedQ, roundedR, roundedS));

        return new Vector3((float)roundedQ, (float)roundedR, (float)roundedS);
    }

    static Vector2 cubeToAxial(Vector3 Hex){
        return new Vector2(Hex[0], Hex[1]);
    }

    static Vector3 axialToCube(Vector2 Hex){
        float cubeQ = Hex[0];
        float cubeR = Hex[1];
        float cubeS = -Hex[0]-Hex[1];
        return new Vector3(cubeQ, cubeR, cubeS);
    }

    static Vector2 axialToOffset(Vector2 hex){
        GD.Print(string.Format("Going to Offset: Hex x {0}, Hex y {1}, modulus {2} | {3}", hex[0], hex[1], Mathf.Abs(hex[0]) % 2.0f, (hex[0] - (Mathf.Abs(hex[0]) % 2f)) / 2f));
        float column = hex[0];
        float row = hex[1] + (hex[0] - (Mathf.Abs(hex[0]) % 2f)) / 2f;
        GD.Print(string.Format("Calculated Offset: Column {0}, Row {1}", column, row));
        return new Vector2(column, row);
    }

    static Vector2 offsetToAxial(Vector2 hex){
        float q = hex[0];
        float r = hex[1] - (hex[0] - (Mathf.Abs(hex[1]) % 2f)) / 2f;
        GD.Print(string.Format("Offset to Axial Conv: Q: {0} | R: {1}", q, r));
        return new Vector2(q, r);
    }
}*/
