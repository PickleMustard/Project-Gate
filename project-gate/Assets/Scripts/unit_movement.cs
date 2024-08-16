using System.Collections;
using Godot;
using HCoroutines;

public partial class unit_movement : Node3D
{
  [Signal]
  public delegate void UpdateSelectedCharacterEventHandler();

  [Export]
  public float movementTime = 1.0f;

  [Export]
  public float rotationDuration = 1.0f;

  private Godot.Collections.Array path = new Godot.Collections.Array { };
  private Node TileGrid;
  private Node level;
  private Node tile;
  private int distance;
  private Vector2I desired_location;

  private Callable UpdateCharacter;
  private Character CurrentCharacter;
  Godot.GodotObject test;
  Node3D capsule;

  public override void _Ready()
  {
    Callable notify = new Callable(this, "NotifyLog");
    UpdateCharacter = new Callable(this, "UpdateCurrentCharacter");
    InputHandler i_handle = GetNode<Node>("/root/Top/input_handler") as InputHandler;
    i_handle.DisplayDestinations += DisplayPotentialDestinations;
    capsule = GetNode<Node3D>("/root/Top/character");
    test = Engine.GetSingleton("GlobalTileNotifier");
    var signals = test.GetSignalList();
    test.Connect(signals[0]["name"].ToString(), notify);
    level = GetNode<Node>("/root/Top/Level");
    GD.Print("Level Children: ", level.GetChildCount());
    TileGrid = level.GetChildren()[0];
    GD.Print("Tile Grid: ", TileGrid.Name);
    Vector2I unit_location = new Vector2I(0, 0);
  }

  public Callable GetUpdateCharacterSignal()
  {
    return UpdateCharacter;
  }

  public void UpdateCurrentCharacter(Character UpdateCharacter)
  {
    CurrentCharacter = UpdateCharacter;
    GD.Print("The Signal works!");
  }


  public override void _Process(double delta)
  {


  }

  public void DisplayPotentialDestinations()
  {
    if (!CurrentCharacter.isMoving)
    {
      Vector2I unit_location = new Vector2I(0, 0);
      if (TileGrid.HasMethod("GetCoordinateFromPosition"))
      {
        unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", capsule.Position, 3.0f);
      }
      Godot.Collections.Array MovementRange = CalculateMovementRange(unit_location, CurrentCharacter.GetDistanceRemaining());
      for (int i = 0; i < MovementRange.Count; i++)
      {
        GodotObject temp = MovementRange[i].AsGodotObject();
        if (temp.HasMethod("set_instance_shader_parameter"))
        {
          temp.Call("set_instance_shader_parameter", "is_clickable", true);
        }
      }
    }
    /*string formated_tile_name = string.Format("/root/Top/Level/{0}/Hex {1},{2}", TileGrid.Name, unit_location[0], unit_location[1]);
    var find_tile = GetNode<Node>(formated_tile_name);
    var MeshInst = find_tile.GetChildren()[1];
    if (MeshInst.HasMethod("set_instance_shader_parameter"))
    {
      MeshInst.Call("set_instance_shader_parameter", "is_clickable", true);
    }*/
  }

  public Godot.Collections.Array CalculateMovementRange(Vector2I center_tile, int radius)
  {
    Godot.Collections.Array MovementRange = new Godot.Collections.Array();
    string formated_tile_name = string.Format("/root/Top/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0], center_tile[1]);
    Node found_tile = GetNode<Node>(formated_tile_name);
    MovementRange.Add(found_tile.GetChildren()[1]);
    for (int q = -radius; q <= radius; q++)
    {
      int r1 = Mathf.Max(-radius, -q - radius);
      int r2 = Mathf.Min(radius, -q + radius);
      for (int r = r1; r <= r2; r++)
      {
        string potential_formated_tile_name = string.Format("/root/Top/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0] + q, center_tile[1] + r);
        found_tile = GetNodeOrNull<Node>(potential_formated_tile_name);
        if (found_tile != null)
        {
          MovementRange.Add(found_tile.GetChildren()[1]);
        }
      }
    }

    return MovementRange;
  }

  public void NotifyLog(Node tile_collider)
  {
    if (!CurrentCharacter.isMoving)
    {
      Vector2I unit_location = new Vector2I(0, 0);
      string tile_name = tile_collider.Name;

      int divider = tile_name.Find(",");
      int q = tile_name.Substring(4, divider - 4).ToInt();
      int r = tile_name.Substring(divider + 1).ToInt();

      if (TileGrid.HasMethod("GetCoordinateFromPosition"))
      {
        unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", capsule.Position, 3.0f);
      }
      Godot.Collections.Array MovementRange = CalculateMovementRange(unit_location, CurrentCharacter.GetDistanceRemaining());
      for (int i = 0; i < MovementRange.Count; i++)
      {
        GodotObject temp = MovementRange[i].AsGodotObject();
        if (temp.HasMethod("set_instance_shader_parameter"))
        {
          temp.Call("set_instance_shader_parameter", "is_clickable", false);
        }
      }
      tile = tile_collider.GetParent();
      if (tile.HasMethod("GetCoordinateFromPosition"))
      {
        unit_location = (Vector2I)tile.Call("GetCoordinateFromPosition", capsule.Position, 3.0f);
      }
      if (tile.HasMethod("GetPositionForHexFromCoordinate"))
      {
        Vector3 location = (Vector3)tile.Call("GetPositionForHexFromCoordinate", new Vector2I(q, r), 3.0f, false);
        desired_location = new Vector2I(q, r);
        if (tile.HasMethod("CalculatePath"))
        {
          path = (Godot.Collections.Array)tile.Call("CalculatePath", unit_location, desired_location);
        }
      }
      if(TileGrid.HasMethod("CalculateDistance")) {
        distance = (int)TileGrid.Call("CalculateDistance", new Vector2I(q, r), desired_location);
      }
      var outside_range = tile_collider.GetChildren()[1];
      if (MovementRange.Contains(outside_range))
      {
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
            CurrentCharacter.isMoving = true;
            CurrentCharacter.DecrementDistanceRemaining(distance);
            GD.Print(CurrentCharacter.GetDistanceRemaining());
            Co.Run(PrepareMovement(location_v3));
          }
        }
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
      float timeElapsed = 0.0f;
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
    float timeElapsed = 0.0f;

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
    } else {
      CurrentCharacter.isMoving = false;
    }
  }
}

