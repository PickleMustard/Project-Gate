using System.Collections;
using Godot;
using HCoroutines;

public partial class unit_movement : Node3D
{
  private int movementTime = 1;
  private int rotationDuration = 1;

  private Godot.Collections.Array path = new Godot.Collections.Array { };
  private Node tile;
  Godot.GodotObject test;
  Node3D capsule;

  public override void _Ready()
  {
    Callable notify = new Callable(this, "NotifyLog");
    capsule = GetNode<Node3D>("/root/Top/character");
    test = Engine.GetSingleton("GlobalTileNotifier");
    var signals = test.GetSignalList();
    test.Connect(signals[0]["name"].ToString(), notify);
  }

  public void NotifyLog(Node tile_collider)
  {
    Vector2I unit_location = new Vector2I(0,0);
    string tile_name = tile_collider.Name;

    int divider = tile_name.Find(",");
    int q = tile_name.Substring(4, divider - 4).ToInt();
    int r = tile_name.Substring(divider + 1).ToInt();

    tile = tile_collider.GetParent();
    if (tile.HasMethod("GetCoordinateFromPosition"))
    {
      unit_location = (Vector2I)tile.Call("GetCoordinateFromPosition", capsule.Position, 3.0f);
    }
    if (tile.HasMethod("GetPositionForHexFromCoordinate"))
    {
      Vector3 location = (Vector3)tile.Call("GetPositionForHexFromCoordinate", new Vector2I(q, r), 3.0f, false);
      Vector2 desired_location = new Vector2I(q, r);
      if (tile.HasMethod("CalculatePath"))
      {
        path = (Godot.Collections.Array)tile.Call("CalculatePath", unit_location, desired_location);
      }
    }
    if (path.Count > 0)
    {
      Variant current_tile_var = path[0];
      path.Remove(current_tile_var);
      GodotObject current_tile = current_tile_var.AsGodotObject();
      if (current_tile.HasMethod("GetLocation"))
      {
        Vector2I location = (Vector2I)current_tile.Call("GetLocation");
        GD.Print("Moving to location: ", location);
        Vector3 location_v3 = (Vector3)tile.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
        Co.Run(PrepareMovement(location_v3));
      }
    }
  }

  private IEnumerator PrepareMovement(Vector3 endPosition)
  {
    Quaternion startRotation = capsule.Quaternion;
    endPosition.Y = capsule.Position.Y;
    Vector3 direction = endPosition - capsule.Position;
    Basis endRotation = Basis.LookingAt(direction);
    Quaternion endQuat = endRotation.GetRotationQuaternion();
    if (Mathf.IsEqualApprox(Mathf.Abs(startRotation.Dot(endQuat)), 1.0f) == false)
    {
      float timeElapsed = 0;
      while (timeElapsed < rotationDuration)
      {
        timeElapsed += Co.DeltaTime;
        float lerpStep = timeElapsed / rotationDuration;
        capsule.Quaternion = Quaternion.Slerp(endQuat, lerpStep);
        yield return null;
      }
      capsule.Quaternion = endQuat;
    }
    Co.Run(MoveUnitAlongTile(endPosition));
  }

  private IEnumerator MoveUnitAlongTile(Vector3 endPosition)
  {
    Vector3 startPosition = capsule.Position;
    float timeElapsed = 0;

    while (timeElapsed < movementTime)
    {
      timeElapsed += Co.DeltaTime;
      float lerpStep = timeElapsed / movementTime;
      capsule.Position = capsule.Position.Lerp(endPosition, lerpStep);
      yield return null;
    }

    capsule.Position = endPosition;
    if (path.Count > 0)
    {
      Variant current_tile_var = path[0];
      path.Remove(current_tile_var);
      GodotObject current_tile = current_tile_var.AsGodotObject();
      if (current_tile.HasMethod("GetLocation"))
      {
        Vector2I location = (Vector2I)current_tile.Call("GetLocation");
        GD.Print("Moving to location: ", location);
        Vector3 location_v3 = (Vector3)tile.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
        Co.Run(PrepareMovement(location_v3));
      }
    }
  }
}

