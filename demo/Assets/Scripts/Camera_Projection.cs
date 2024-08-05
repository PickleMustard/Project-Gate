using Godot;
using System;

public partial class Camera_Projection : Node3D
{
  [Export]
  public Camera3D camera;

  public override void _Ready()
  {
    Callable notify = new Callable(this, "NotifyLog");
    //input_handler i_handle = GetNode<Node>("/root/true_parent/input_handler") as input_handler;
    //i_handle.PickTile += FindTileAtPosition;
    var test = Engine.GetSingleton("GlobalTileNotifier");
    var testTimer = new Timer();
    var signals = test.GetSignalList();
    test.Connect(signals[0]["name"].ToString(), notify);
    GD.Print(signals[0]["name"].ToString());
    GD.Print(test.HasSignal("TileSelected"));
    GD.Print(test.GetType());
    GD.Print(testTimer.GetType());
    GD.Print(signals);

  }


  public void FindTileAtPosition(Vector2 mouse_position)
  {
    Vector3 projection_vector_origin = camera.ProjectRayOrigin(mouse_position);
    Vector3 projection_vector = projection_vector_origin + camera.ProjectRayNormal(mouse_position) * 1000000000.0f;
    PhysicsRayQueryParameters3D query = PhysicsRayQueryParameters3D.Create(projection_vector_origin, projection_vector);
    Godot.Collections.Dictionary collision = GetWorld3D().DirectSpaceState.IntersectRay(query);
    GD.Print($"Query: {collision}");
  }

  public void NotifyLog(Node tile_collider)
  {
    GD.Print("Something");
    GD.Print(tile_collider.GetParent().ToString());
    Node tile = tile_collider.GetParent();
  }
}
