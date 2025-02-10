using System;
using System.Collections;
using HCoroutines;
using Godot;

using ProjGate.TargetableEntities;

namespace ProjGate.Services
{
    public partial class MovementSystem : Node
    {
        [Export]
        public float movementTime = 1.0f;

        [Export]
        public float rotationDuration = 1.0f;

        private BaseCharacter currentCharacter;
        private Node TileGrid;
        private Godot.Collections.Array path = new Godot.Collections.Array { };
        private Node tile;

        public override void _EnterTree()
        {
            AddToGroup("UnitControl");
        }
        public override void _Ready()
        {
          CommunicationBus.Instance.LevelGenerated += OnLevelReady;
        }

        public void OnLevelReady()
        {
            TileGrid = GetTree().GetNodesInGroup("Tilegrid")[0];
        }
        public void DisplayPotentialDestinations(BaseCharacter CurrentCharacter)
        {
            if (!CurrentCharacter.isMoving)
            {
                Godot.Collections.Array MovementRange = GetPotentialDestinations(CurrentCharacter);
                for (int i = 0; i < MovementRange.Count; i++)
                {
                    MeshInstance3D tileMesh = ((Godot.Collections.Dictionary)MovementRange[i])["TileMesh"].AsGodotObject() as MeshInstance3D;
                    bool hasEnemy = false;
                    if (((Godot.Collections.Dictionary)MovementRange[i]).ContainsKey("CharacterType"))
                    {
                        hasEnemy = (int)((Godot.Collections.Dictionary)MovementRange[i])["CharacterType"] == (int)BaseCharacter.CHARACTER_TEAM.enemy;
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
            }
        }

        public void HidePotentialDestinations(BaseCharacter CurrentCharacter)
        {
            if (!CurrentCharacter.isMoving)
            {
                Vector2I unit_location = new Vector2I(0, 0);
                if (TileGrid.HasMethod("GetCoordinateFromPosition"))
                {
                    unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
                }
                Godot.Collections.Array MovementRange = CalculateCharacterMovementRange(unit_location, Mathf.Max(CurrentCharacter.MainWeapon.GetMaxRange(), CurrentCharacter.GetDistanceRemaining()));
                for (int i = 0; i < MovementRange.Count; i++)
                {
                    MeshInstance3D tileMesh = ((Godot.Collections.Dictionary)MovementRange[i])["TileMesh"].AsGodotObject() as MeshInstance3D;
                    Material activeMaterial = tileMesh.GetActiveMaterial(0);
                    activeMaterial.NextPass = null;
                }
            }
        }

        public Godot.Collections.Array GetPotentialDestinations(BaseCharacter CurrentCharacter)
        {
            Vector2I unit_location = new Vector2I(0, 0);
            if (TileGrid.HasMethod("GetCoordinateFromPosition"))
            {
                unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
            }
            return CalculateCharacterMovementRange(unit_location, CurrentCharacter.GetDistanceRemaining());
        }

        public Godot.Collections.Array GetPotentialDestinations(BaseCharacter CurrentCharacter, int radius)
        {
            Vector2I unit_location = new Vector2I(0, 0);
            if (TileGrid.HasMethod("GetCoordinateFromPosition"))
            {
                unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
            }
            return CalculateCharacterMovementRange(unit_location, radius);
        }

        public Godot.Collections.Array CalculateCharacterMovementRange(Vector2I center_tile, int radius)
        {
            Godot.Collections.Array MovementRange = new Godot.Collections.Array();
            string formated_tile_name = string.Format("/root/Level/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0], center_tile[1]);
            Node found_tile;// = GetNode<Node>(formated_tile_name);
            for (int q = -radius; q <= radius; q++)
            {
                int r1 = Mathf.Max(-radius, -q - radius);
                int r2 = Mathf.Min(radius, -q + radius);
                for (int r = r1; r <= r2; r++)
                {
                    string potential_formated_tile_name = string.Format("/root/Level/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0] + q, center_tile[1] + r);
                    found_tile = GetNodeOrNull<Node>(potential_formated_tile_name);
                    if (found_tile != null && found_tile.GetChildCount() > 0)
                    {
                        GodotObject TileReference = new GodotObject();
                        BaseCharacter possibleCharacter = null;
                        if (TileGrid.HasMethod("FindTileOnGrid"))
                        {
                            TileReference = TileGrid.Call("FindTileOnGrid", new Vector2I(center_tile[0] + q, center_tile[1] + r)).AsGodotObject();
                        }

                        if (TileReference.HasMethod("GetCharacterOnTile"))
                        {
                            possibleCharacter = TileReference.Call("GetCharacterOnTile").AsGodotObject() as BaseCharacter;
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
            string formated_tile_name = string.Format("/root/Level/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0], center_tile[1]);
            Node found_tile = GetNode<Node>(formated_tile_name);
            for (int q = -radius; q <= radius; q++)
            {
                int r1 = Mathf.Max(-radius, -q - radius);
                int r2 = Mathf.Min(radius, -q + radius);
                for (int r = r1; r <= r2; r++)
                {
                    string potential_formated_tile_name = string.Format("/root/Level/Level/{0}/Hex {1},{2}", TileGrid.Name, center_tile[0] + q, center_tile[1] + r);
                    found_tile = GetNodeOrNull<Node>(potential_formated_tile_name);
                    if (found_tile != null && found_tile.GetChildCount() > 0)
                    {
                        MovementRange.Add(found_tile.GetChildren()[1]);
                    }
                }
            }

            return MovementRange;
        }

        public void MoveCharacter(BaseCharacter CurrentCharacter, Node tile_collider)
        {
            this.currentCharacter = CurrentCharacter;
            this.tile = tile_collider.GetParent();
            if (!CurrentCharacter.isMoving)
            {
                Vector2I unit_location = new Vector2I(0, 0);
                string tile_name = tile_collider.Name;

                int divider = tile_name.Find(",");
                int q = tile_name.Substring(4, divider - 4).ToInt();
                int r = tile_name.Substring(divider + 1).ToInt();

                Node tile;

                if (TileGrid.HasMethod("GetCoordinateFromPosition"))
                {
                    unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
                }
                //HidePotentialDestinations();
                Godot.Collections.Array MovementRange = CalculateMovementRange(unit_location, CurrentCharacter.GetDistanceRemaining());
                tile = tile_collider.GetParent();
                Vector2I desired_location = new Vector2I(q, r);
                int distance = 0;
                if (tile.HasMethod("GetPositionForHexFromCoordinate"))
                {
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
                            Vector3 location_v3 = (Vector3)tile.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
                            CurrentCharacter.isMoving = true;
                            CurrentCharacter.DecrementDistanceRemaining(distance);
                            Co.Run(PrepareMovement(location_v3));
                        }
                    }
                }
            }
        }

        private IEnumerator PrepareMovement(Vector3 endPosition)
        {
            Quaternion startRotation = currentCharacter.Quaternion;
            endPosition.Y = currentCharacter.Position.Y;
            Vector3 direction = endPosition - currentCharacter.Position;
            Basis endRotation = Basis.LookingAt(direction);
            Quaternion endQuat = endRotation.GetRotationQuaternion();
            Vector2I unit_location = new Vector2I(0, 0);
            if (TileGrid.HasMethod("GetCoordinateFromPosition"))
            {
                unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", currentCharacter.Position, 3.0f);
            }

            if (TileGrid.HasMethod("FindTileOnGrid"))
            {
                var temp = TileGrid.Call("FindTileOnGrid", unit_location);
                var tempObject = temp.AsGodotObject();
                if (tempObject.HasMethod("SetCharacterOnTile"))
                {
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
                    currentCharacter.Quaternion = currentCharacter.Quaternion.Slerp(endQuat, lerpStep);
                    yield return null;
                }
                currentCharacter.Quaternion = endQuat;
            }
            Co.Run(MoveUnitAlongTile(endPosition));
        }

        private IEnumerator MoveUnitAlongTile(Vector3 endPosition)
        {
            Vector3 startPosition = currentCharacter.Position;
            float timeElapsed = 0.0f;
            Vector2I unit_location = new Vector2I(0, 0);

            while (timeElapsed < movementTime)
            {
                timeElapsed += Co.DeltaTime;
                float lerpStep = timeElapsed / movementTime;
                currentCharacter.Position = currentCharacter.Position.Lerp(endPosition, lerpStep);
                yield return null;
            }

            currentCharacter.Position = endPosition;
            if (TileGrid.HasMethod("GetCoordinateFromPosition"))
            {
                unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", currentCharacter.Position, 3.0f);
            }

            if (TileGrid.HasMethod("FindTileOnGrid"))
            {
                var temp = TileGrid.Call("FindTileOnGrid", unit_location);
                var tempObject = temp.AsGodotObject();
                if (tempObject.HasMethod("SetCharacterOnTile"))
                {
                    tempObject.Call("SetCharacterOnTile", currentCharacter);
                }
                if (tempObject.HasMethod("GetCharacterOnTile"))
                {
                    //GD.Print(tempObject.Call("GetCharacterOnTile").AsGodotObject().GetMethodList());
                }
                /*if (tempObject.HasMethod("TileSteppedOnEvent"))
                {
                  tempObject.Call("TileSteppedOnEvent");
                }*/
            }

            if (path.Count > 0)
            {
                Variant current_tile_var = path[0];
                path.Remove(current_tile_var);
                GodotObject current_tile = current_tile_var.AsGodotObject();
                if (current_tile.HasMethod("GetLocation"))
                {
                    Vector2I location = (Vector2I)current_tile.Call("GetLocation");
                    Vector3 location_v3 = (Vector3)tile.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
                    Co.Run(PrepareMovement(location_v3));
                }
            }
            else
            {
                currentCharacter.isMoving = false;
            }
        }
    }
}
