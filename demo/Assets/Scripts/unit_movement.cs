using Godot;

public partial class unit_movement :  Node3D {
  private int counter = 0;

  Godot.GodotObject test;

  public override void _Ready()
  {
    GD.Print("Unit Movement");
    Callable notify = new Callable(this, "NotifyLog");
    //input_handler i_handle = GetNode<Node>("/root/true_parent/input_handler") as input_handler;
    //i_handle.PickTile += FindTileAtPosition;
    test = Engine.GetSingleton("GlobalTileNotifier");
    var testTimer = new Timer();
    var signals = test.GetSignalList();
    test.Connect(signals[0]["name"].ToString(), notify);
    GD.Print(signals[0]["name"].ToString());
    GD.Print(test.HasSignal("TileSelected"));
    GD.Print(test.GetType());
    GD.Print(testTimer.GetType());
    GD.Print(signals);





  }

  public void NotifyLog(Node tile_collider)
  {
    GD.Print("Something");
    GD.Print($"Counter: %d", counter);
    counter++;
    GD.Print(tile_collider.GetParent().ToString());
    Node tile = tile_collider.GetParent();
    //GD.Print(tile.GetMethodList());
    if (tile.HasMethod("GetPositionForhexFromCoordinate")) {
      GD.Print("Hahahaha");
      //GD.Print(test.Call("GetPositionForHexFromCoordinate"));
    }
  }

}
