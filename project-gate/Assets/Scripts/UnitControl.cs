using System.Collections;
using Godot;
using HCoroutines;

public partial class UnitControl : Node3D
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
  private Vector2I unit_location;
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
    i_handle.HideDestinations += HidePotentialDestinations;
    //capsule = GetNode<Node3D>("/root/Top/character");
    test = Engine.GetSingleton("GlobalTileNotifier");
    var signals = test.GetSignalList();
    test.Connect(signals[0]["name"].ToString(), notify);
    level = GetNode<Node>("/root/Top/Level");
    unit_location = new Vector2I(0, 0);
    AddToGroup("UnitControl");
  }

  public Callable GetUpdateCharacterSignal()
  {
    return UpdateCharacter;
  }

  public void UpdateCurrentCharacter(Character UpdateCharacter)
  {
    CurrentCharacter = UpdateCharacter;
    GD.Print("Movement Character: ", CurrentCharacter.ToString());
  }


  public void HidePotentialDestinations()
  {
    if (!CurrentCharacter.isMoving)
    {
      Godot.Collections.Array MovementRange = CalculateCharacterMovementRange(unit_location, Mathf.Max(CurrentCharacter.MainWeapon.GetMaxRange(), CurrentCharacter.GetDistanceRemaining()));
      for (int i = 0; i < MovementRange.Count; i++)
      {
        MeshInstance3D tileMesh = ((Godot.Collections.Dictionary)MovementRange[i])["TileMesh"].AsGodotObject() as MeshInstance3D;
        Material activeMaterial = tileMesh.GetActiveMaterial(0);
        activeMaterial.NextPass = null;
      }
    }
  }


  public void DisplayPotentialDestinations()
  {
    if (!CurrentCharacter.isMoving)
    {
      Godot.Collections.Array MovementRange = GetPotentialDestinations();
      for (int i = 0; i < MovementRange.Count; i++)
      {
        MeshInstance3D tileMesh = ((Godot.Collections.Dictionary)MovementRange[i])["TileMesh"].AsGodotObject() as MeshInstance3D;
        bool hasEnemy = false;
        if (((Godot.Collections.Dictionary)MovementRange[i]).ContainsKey("CharacterType"))
        {
          hasEnemy = (int)((Godot.Collections.Dictionary)MovementRange[i])["CharacterType"] == (int)Character.CHARACTER_TEAM.enemy;
        }
        string locationString = tileMesh.GetParent().Name;
        Material activeMaterial = tileMesh.GetActiveMaterial(0);
        Material blinkingDisplay = (ResourceLoader.Load("res://Assets/Materials/RangeIndicator.tres")).Duplicate() as Material;
        if (hasEnemy)
        {
          blinkingDisplay.Call("set_shader_parameter", "color", new Color(.5f, 0.0f, 0.0f, 0.0f));
        }
        blinkingDisplay.ResourceLocalToScene = true;
        activeMaterial.NextPass = blinkingDisplay;
      }
      if (CurrentCharacter.MainWeapon.GetMaxRange() > CurrentCharacter.GetDistanceRemaining())
      {
        Godot.Collections.Array WeaponRange = GetPotentialDestinations(CurrentCharacter.MainWeapon.GetMaxRange());
        for (int i = 0; i < WeaponRange.Count; i++)
        {
          if (!MovementRange.Contains(WeaponRange[i]))
          {
            MeshInstance3D tileMesh = (MeshInstance3D)((Godot.Collections.Dictionary)WeaponRange[i])["TileMesh"].AsGodotObject();
            Material activeMaterial = tileMesh.GetActiveMaterial(0);
            Material blinkingDisplay = (ResourceLoader.Load("res://Assets/Materials/RangeIndicator.tres")).Duplicate() as Material;
            blinkingDisplay.Call("set_shader_parameter", "color", new Color(.5f, 0.0f, 0.0f, 0.0f));
            activeMaterial.NextPass = blinkingDisplay;
          }
        }

      }
    }
  }

  public Godot.Collections.Array GetPotentialDestinations()
  {
    unit_location = new Vector2I(0, 0);
    TileGrid = GetTree().GetNodesInGroup("Tilegrid")[0];
    if (TileGrid.HasMethod("GetCoordinateFromPosition"))
    {
      unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
    }
    return CalculateCharacterMovementRange(unit_location, CurrentCharacter.GetDistanceRemaining());
  }

  public Godot.Collections.Array GetPotentialDestinations(int radius)
  {
    unit_location = new Vector2I(0, 0);
    TileGrid = GetTree().GetNodesInGroup("Tilegrid")[0];
    if (TileGrid.HasMethod("GetCoordinateFromPosition"))
    {
      unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
    }
    return CalculateCharacterMovementRange(unit_location, radius);
  }

  public Godot.Collections.Array CalculateCharacterMovementRange(Vector2I center_tile, int radius)
  {
    Godot.Collections.Array MovementRange = new Godot.Collections.Array();
    string formated_tile_name = string.Format("/root/Top/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0], center_tile[1]);
    Node found_tile = GetNode<Node>(formated_tile_name);
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
          GodotObject TileReference = new GodotObject();
          Character possibleCharacter = null;
          if (TileGrid.HasMethod("FindTileOnGrid"))
          {
            TileReference = TileGrid.Call("FindTileOnGrid", new Vector2I(center_tile[0] + q, center_tile[1] + r)).AsGodotObject();
          }

          if (TileReference.HasMethod("GetCharacterOnTile"))
          {
            possibleCharacter = TileReference.Call("GetCharacterOnTile").AsGodotObject() as Character;
          }
          Godot.Collections.Dictionary tileInformation = new Godot.Collections.Dictionary();
          tileInformation["TileMesh"] = found_tile.GetChildren()[1];
          if (possibleCharacter != null)
          {
            tileInformation["CharacterType"] = (int)possibleCharacter.team;
          }
          MovementRange.Add(tileInformation);
        }
      }
    }

    return MovementRange;

  }

  public Godot.Collections.Array CalculateMovementRange(Vector2I center_tile, int radius)
  {
    Godot.Collections.Array MovementRange = new Godot.Collections.Array();
    string formated_tile_name = string.Format("/root/Top/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0], center_tile[1]);
    Node found_tile = GetNode<Node>(formated_tile_name);
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
    unit_location = new Vector2I(0, 0);
    string tile_name = tile_collider.Name;
    tile = tile_collider.GetParent();

    int divider = tile_name.Find(",");
    int q = tile_name.Substring(4, divider - 4).ToInt();
    int r = tile_name.Substring(divider + 1).ToInt();
    Vector2I DesiredTileLocation = new Vector2I(q, r);
    Vector2I CurrentUnitLocation = new Vector2I(0, 0);
    Variant temp;
    GodotObject tempObject = new GodotObject();
    Variant tempChar;
    if (tile.HasMethod("GetCoordinateFromPosition"))
    {
      CurrentUnitLocation = (Vector2I)tile.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
    }
    GD.Print("Desired Tile Location: ", DesiredTileLocation, "| Current Unit Location: ", CurrentUnitLocation);

    if (TileGrid.HasMethod("FindTileOnGrid"))
    {
      temp = TileGrid.Call("FindTileOnGrid", DesiredTileLocation);
      GD.Print("unit_location ", unit_location);
      tempObject = temp.AsGodotObject();
    }

    if (tempObject.HasMethod("GetCharacterOnTile"))
    {
      tempChar = tempObject.Call("GetCharacterOnTile");
      GD.Print(tempChar.AsGodotObject());
      if (tempChar.AsGodotObject() != null)
      {
        AttackTile(DesiredTileLocation, CurrentUnitLocation, CurrentCharacter);
      }
      else
      {
        MoveCharacter(tile_collider);
      }
    }


  }

  public void AttackTile(Vector2I TargetPosition, Vector2I ShooterLocation, GodotObject attacker)
  {
    GD.Print("Attacking Location");
    Character target = new Character();
    Resource TargetTile = new Resource();
    Resource ShooterTile = new Resource();
    if (TileGrid.HasMethod("FindTileOnGrid"))
    {
      ShooterTile = TileGrid.Call("FindTileOnGrid", ShooterLocation).AsGodotObject() as Resource;
      TargetTile = TileGrid.Call("FindTileOnGrid", TargetPosition).AsGodotObject() as Resource;
      if (TargetTile.HasMethod("GetCharacterOnTile"))
      {
        target = TargetTile.Call("GetCharacterOnTile").AsGodotObject() as Character;
        //GD.Print("Getting character on tile: ", TargetTile, " | ", target);
        if (target == null || !target.GetType().IsSubclassOf(System.Type.GetType("Character")))
        {
          GD.PushError("Could not get the Character: ", target, " on Tile: ", TargetTile);
          return;
        }
      }
    }

    if (TileGrid.HasMethod("CalculateDistance"))
    {
      int distance = (int)TileGrid.Call("CalculateDistance", ShooterLocation, TargetPosition);
      GD.Print("Distance between attacker and target: ", distance);
      GD.Print(attacker);
      //GD.Print(attacker.Call("GetMainWeapon"));
      if (distance <= (int)((attacker.Call("GetMainWeapon").AsGodotObject()).Call("GetMaxRange")))
      {
        GD.Print("Target: ", target, "| Has AttackCharacter Method: ", target.HasMethod("AttackCharacter"));
        if (target.HasMethod("AttackCharacter") && target.team != CurrentCharacter.team)
        {
          CurrentCharacter.AttackCharacter(TargetTile);
        }
      }
    }
  }

  public void MoveCharacter(Node tile_collider)
  {
    GD.Print("Main Weapon:", CurrentCharacter.GetMainWeapon());
    if (!CurrentCharacter.isMoving)
    {
      unit_location = new Vector2I(0, 0);
      string tile_name = tile_collider.Name;

      int divider = tile_name.Find(",");
      int q = tile_name.Substring(4, divider - 4).ToInt();
      int r = tile_name.Substring(divider + 1).ToInt();


      if (TileGrid.HasMethod("GetCoordinateFromPosition"))
      {
        unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
      }
      HidePotentialDestinations();
      Godot.Collections.Array MovementRange = CalculateMovementRange(unit_location, CurrentCharacter.GetDistanceRemaining());
      tile = tile_collider.GetParent();
      if (tile.HasMethod("GetPositionForHexFromCoordinate"))
      {
        desired_location = new Vector2I(q, r);
        if (tile.HasMethod("CalculatePath"))
        {
          path = (Godot.Collections.Array)tile.Call("CalculatePath", unit_location, desired_location);
        }
      }
      if (TileGrid.HasMethod("CalculateDistance"))
      {
        distance = (int)TileGrid.Call("CalculateDistance", unit_location, desired_location);
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
            GD.Print("Removing ", distance, " tiles");
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

    Quaternion startRotation = CurrentCharacter.Quaternion;
    endPosition.Y = CurrentCharacter.Position.Y;
    Vector3 direction = endPosition - CurrentCharacter.Position;
    Basis endRotation = Basis.LookingAt(direction);
    Quaternion endQuat = endRotation.GetRotationQuaternion();
    if (TileGrid.HasMethod("GetCoordinateFromPosition"))
    {
      unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
    }

    if (TileGrid.HasMethod("FindTileOnGrid"))
    {
      var temp = TileGrid.Call("FindTileOnGrid", unit_location);
      var tempObject = temp.AsGodotObject();
      if (tempObject.HasMethod("SetCharacterOnTile"))
      {
        GD.Print("Resetting character");
        tempObject.Call("ResetCharacterOnTile");
      }
    }

    if (Mathf.IsEqualApprox(Mathf.Abs(startRotation.Dot(endQuat)), 1.0f) == false)
    {
      float timeElapsed = 0.0f;
      while (timeElapsed < rotationDuration)
      {
        timeElapsed += Co.DeltaTime;
        float lerpStep = timeElapsed / rotationDuration;
        CurrentCharacter.Quaternion = Quaternion.Slerp(endQuat, lerpStep);
        yield return null;
      }
      CurrentCharacter.Quaternion = endQuat;
    }
    Co.Run(MoveUnitAlongTile(endPosition));
  }

  private IEnumerator MoveUnitAlongTile(Vector3 endPosition)
  {
    GD.Print("End Position: ", endPosition, "| ", CurrentCharacter.isMoving);
    Vector3 startPosition = CurrentCharacter.Position;
    float timeElapsed = 0.0f;

    while (timeElapsed < movementTime)
    {
      timeElapsed += Co.DeltaTime;
      float lerpStep = timeElapsed / movementTime;
      CurrentCharacter.Position = CurrentCharacter.Position.Lerp(endPosition, lerpStep);
      yield return null;
    }

    CurrentCharacter.Position = endPosition;
    if (TileGrid.HasMethod("GetCoordinateFromPosition"))
    {
      unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
    }

    if (TileGrid.HasMethod("FindTileOnGrid"))
    {
      var temp = TileGrid.Call("FindTileOnGrid", unit_location);
      var tempObject = temp.AsGodotObject();
      if (tempObject.HasMethod("SetCharacterOnTile"))
      {
        GD.Print("Setting character");
        tempObject.Call("SetCharacterOnTile", CurrentCharacter);
      }
      if (tempObject.HasMethod("GetCharacterOnTile"))
      {
        //GD.Print(tempObject.Call("GetCharacterOnTile").AsGodotObject().GetMethodList());
      }
      if (tempObject.HasMethod("TileSteppedOnEvent"))
      {
        GD.Print("TileSteppedOnEvent exists, calling");
        tempObject.Call("TileSteppedOnEvent");
      }
    }

    if (path.Count > 0)
    {
      Variant current_tile_var = path[0];
      path.Remove(current_tile_var);
      GodotObject current_tile = current_tile_var.AsGodotObject();
      GD.Print("Location has character on it: ", current_tile.Call("HasCharacterOnTile"));
      if (current_tile.HasMethod("GetLocation"))
      {
        Vector2I location = (Vector2I)current_tile.Call("GetLocation");
        GD.Print("Moving to location: ", location);
        Vector3 location_v3 = (Vector3)tile.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
        Co.Run(PrepareMovement(location_v3));
      }
    }
    else
    {
      GD.Print("Done Moving");
      CurrentCharacter.isMoving = false;
    }
  }
}

