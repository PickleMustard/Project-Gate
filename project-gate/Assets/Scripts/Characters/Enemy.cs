using Godot;
using System;

public partial class Enemy : Character
{
  Node TileGrid;
  Node level;

  Func<Node> test;
  public override void _Ready()
  {
    Callable SetPositionCall = new Callable(this, "SetPosition");
    level = GetNode<Node>("/root/Top/Level");
    TileGrid = level.GetChildren()[0];
    if (TileGrid.HasMethod("AddEnemyCall"))
    {
      TileGrid.Call("AddEnemyCall", SetPositionCall);
    }
    HealCharacter(TotalHealth);
    GD.Print("Health at beninging ", currentHealth);
  }



  public void SetPosition(Resource Tile)
  {
    Vector2I location = new Vector2I();
    GD.Print("Finished Awaiting");
    if(Tile.HasMethod("GetLocation")) {
      GD.Print("Location ", Tile.Call("GetLocation"));
      location = (Vector2I)Tile.Call("GetLocation");
    }
    if(TileGrid.HasMethod("GetPositionForHexFromCoordinate")) {
      GD.Print("Position: ", TileGrid.Call("GetPositionForHexFromCoordinate", location, (int)Tile.Call("GetSize"), true));
      Position = (Vector3)TileGrid.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
    }
    if (Tile.HasMethod("SetCharacterOnTile"))
    {
      Tile.Call("SetCharacterOnTile", this);
      GD.Print("Setting object");
    }
  }
}
