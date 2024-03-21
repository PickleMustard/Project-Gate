using Godot;

public partial class Tile : Node3D
{
    public enum tileType{
        walkable,
        obstacle,
        wall
    }
    public bool obstacle;
    public Vector3 tilePosition;
    public int row;
    public int column;
    public int gCost;
    public int hCost;
    public Tile parent;
    private bool isFlatTopped;
    private float outerSize;
    private float innerSize;
    private float height;

    public Tile(Vector3 position, int row, int column, bool obstacle, float innerSize, float outerSize, float height, bool isFlatTopped){
        //this.tileObject = tile;
        this.obstacle = obstacle;
        this.Position = position;
        this.row = row;
        this.column = column;
        this.isFlatTopped = isFlatTopped;
        this.outerSize = outerSize;
        this.innerSize = innerSize;
        this.height = height;
        HexagonalTileRenderer tileRenderer  = new HexagonalTileRenderer(innerSize, outerSize, height, isFlatTopped);
        tileRenderer.DrawMesh();
        this.AddChild(tileRenderer);
    }

    public int fCost{
        get {
            return gCost + hCost;
        }
    }
    /*

    public void unitWalkOnTile(){
        walkOnTileBehavior.walkOnTile();
    }

    public void unitStayOnTile(){
        stayOnTileBehavior.stayOnTile();
    }

    public void unitMoveOffTile(){
        moveOffTileBehavior.MoveOffTile();
    }*/
}
