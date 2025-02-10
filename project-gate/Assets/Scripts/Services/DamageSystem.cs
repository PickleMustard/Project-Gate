using Godot;

using ProjGate.TargetableEntities;

namespace ProjGate.Services
{
    public partial class DamageSystem : Node
    {

        private BaseCharacter currentCharacter;
        private Node TileGrid;
        private Godot.Collections.Array path = new Godot.Collections.Array { };

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

        public void DisplayPotentialWeaponTargets(BaseCharacter CurrentCharacter)
        {
            Godot.Collections.Array WeaponRange = GetPotentialTargetableLocations(CurrentCharacter, CurrentCharacter.MainWeapon.GetMaxRange());
            bool hasEnemy;
            for (int i = 0; i < WeaponRange.Count; i++)
            {
                hasEnemy = false;
                if (((Godot.Collections.Dictionary)WeaponRange[i]).ContainsKey("CharacterType"))
                {
                    hasEnemy = (int)((Godot.Collections.Dictionary)WeaponRange[i])["CharacterType"] == (int)BaseCharacter.CHARACTER_TEAM.enemy;
                }
                if (hasEnemy)
                {
                    MeshInstance3D tileMesh = (MeshInstance3D)((Godot.Collections.Dictionary)WeaponRange[i])["TileMesh"].AsGodotObject();
                    Material activeMaterial = tileMesh.GetActiveMaterial(0);
                    Material blinkingDisplay = (ResourceLoader.Load("res://Assets/Materials/RangeIndicator.tres")).Duplicate() as Material;
                    blinkingDisplay.Call("set_shader_parameter", "color", new Color(.5f, 0.0f, 0.0f, 0.0f));
                    activeMaterial.NextPass = blinkingDisplay;
                }
            }

        }

        public Godot.Collections.Array GetPotentialTargetableLocations(BaseCharacter CurrentCharacter, int radius)
        {
            Vector2I unit_location = new Vector2I(0, 0);
            if (TileGrid.HasMethod("GetCoordinateFromPosition"))
            {
                unit_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
            }
            return CalculateCharacterMovementRange(unit_location, radius, TileGrid);
        }

        public Godot.Collections.Array CalculateCharacterMovementRange(Vector2I center_tile, int radius, Node TileGrid)
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

        public void AttackTile(Vector2I TargetPosition, Vector2I ShooterLocation, GodotObject attacker)
        {
            GD.Print("Attacking Location");
            BaseCharacter target = new BaseCharacter();
            Resource TargetTile = new Resource();
            Resource ShooterTile = new Resource();
            if (TileGrid.HasMethod("FindTileOnGrid"))
            {
                ShooterTile = TileGrid.Call("FindTileOnGrid", ShooterLocation).AsGodotObject() as Resource;
                TargetTile = TileGrid.Call("FindTileOnGrid", TargetPosition).AsGodotObject() as Resource;
                if (TargetTile.HasMethod("GetCharacterOnTile"))
                {
                    target.QueueFree();
                    target = TargetTile.Call("GetCharacterOnTile").AsGodotObject() as BaseCharacter;
                    //GD.Print("Getting character on tile: ", TargetTile, " | ", target);
                    if (target == null || !target.GetType().IsSubclassOf(System.Type.GetType("ProjGate.TargetableEntities.TargetableEntity")))
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
                    if (target.HasMethod("AttackCharacter") && target.team != currentCharacter.team)
                    {
                        currentCharacter.AttackCharacter(TargetTile);
                    }
                }
            }
        }
    }
}
