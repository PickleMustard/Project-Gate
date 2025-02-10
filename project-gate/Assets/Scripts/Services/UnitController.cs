using System;
using Godot;
using Godot.Collections;

using ProjGate.TargetableEntities;

namespace ProjGate.Services
{
    public enum InteractionStates
    {
        DisplayMovement=0,
        DisplayWeaponDetails=1,
        DisplayGrenadeDetails=2,
        DisplayAbilityDetails=3,
    }
    public partial class UnitController : Node
    {
        class ControlState
        {
            private bool processingCharacter = false;
            public InteractionStates currentState { get; private set; } = InteractionStates.DisplayMovement;
            private InteractionStates nextState = InteractionStates.DisplayMovement;

            public ControlState()
            {
                currentState = InteractionStates.DisplayMovement;
            }

            public ControlState(InteractionStates startingState)
            {
                currentState = startingState;
            }

            public void UpdateNextState(InteractionStates nextState)
            {
                this.nextState = nextState;
            }

            public bool AttemptChangeState(InteractionStates desiredNextState)
            {
                if (!IsProcessingState())
                {
                    currentState = desiredNextState;
                    if (nextState == InteractionStates.DisplayAbilityDetails || nextState == InteractionStates.DisplayGrenadeDetails)
                        nextState = InteractionStates.DisplayMovement;
                    return true;
                }
                return false;
            }

            public void ChangeState()
            {
                currentState = nextState;
                processingCharacter = false;
            }

            public void StartProcessingState()
            {
                processingCharacter = true;
            }

            public bool IsProcessingState()
            {
                return processingCharacter;
            }

        }
        [Signal]
        public delegate void UpdateSelectedCharacterEventHandler();


        private ControlState displayState;
        private Godot.Collections.Array path = new Godot.Collections.Array { };
        private Node TileGrid;
        private Node level;
        private Node tile;
        private int distance;
        private Vector2I unit_location;
        private Vector2I desired_location;

        private MovementSystem movementSystem;
        private DamageSystem damageSystem;

        public Callable UpdateCharacter { get; private set; }
        private BaseCharacter CurrentCharacter;
        public System.Collections.Generic.Dictionary<string, InteractionStates> InteractionStateStringConverter { get; private set;
        } = new System.Collections.Generic.Dictionary<string, InteractionStates>();
        Godot.GodotObject test;
        Node3D capsule;

        public override void _EnterTree()
        {
            displayState = new ControlState();
            CommunicationBus cb = Engine.GetSingleton("CommunicationBus") as CommunicationBus;
            cb.UpdateUnitControllerDisplayState += UpdateDisplayState;
            cb.ProcessUnitCommand += ProcessUnitOrder;
            AddToGroup("UnitControl");

            InteractionStateStringConverter["DisplayMovement"] = InteractionStates.DisplayMovement;
            InteractionStateStringConverter["DisplayWeaponDetails"] = InteractionStates.DisplayWeaponDetails;
            InteractionStateStringConverter["DisplayGrenadeDetails"] = InteractionStates.DisplayGrenadeDetails;
            InteractionStateStringConverter["DisplayAbilityDetails"] = InteractionStates.DisplayAbilityDetails;
        }

        public override void _Ready()
        {
            Callable notify = new Callable(this, "NotifyLog");
            UpdateCharacter = new Callable(this, "UpdateCurrentCharacter");
            level = GetNode<Node>("/root/Level/Level");
            unit_location = new Vector2I(0, 0);

            Array<Node> controllers = GetTree().GetNodesInGroup("UnitControl");
            damageSystem = controllers[1] as DamageSystem;
            movementSystem = controllers[2] as MovementSystem;

            CommunicationBus.Instance.LevelGenerated += UponLevelStart;
        }

        public int GetInteractionStateFromStringName(string Name) {
          return (int)InteractionStateStringConverter[Name];
        }

        public void UponLevelStart()
        {
            TileGrid = GetTree().GetNodesInGroup("Tilegrid")[0];
        }

        public void UpdateDisplayState(InteractionStates nextState)
        {
            switch (nextState)
            {
                case InteractionStates.DisplayMovement:
                    if (!displayState.AttemptChangeState(InteractionStates.DisplayMovement))
                    {
                        displayState.UpdateNextState(InteractionStates.DisplayMovement);
                    }
                    break;

                case InteractionStates.DisplayWeaponDetails:
                    if (!displayState.AttemptChangeState(InteractionStates.DisplayWeaponDetails))
                    {
                        displayState.UpdateNextState(InteractionStates.DisplayWeaponDetails);
                    }
                    break;

                case InteractionStates.DisplayGrenadeDetails:
                    if (!displayState.AttemptChangeState(InteractionStates.DisplayGrenadeDetails))
                    {
                        displayState.UpdateNextState(InteractionStates.DisplayGrenadeDetails);
                    }
                    break;

                case InteractionStates.DisplayAbilityDetails:
                    if (!displayState.AttemptChangeState(InteractionStates.DisplayAbilityDetails))
                    {
                        displayState.UpdateNextState(InteractionStates.DisplayAbilityDetails);
                    }
                    break;
            }
        }

        public void UpdateCurrentCharacter(BaseCharacter UpdateCharacter)
        {
            CurrentCharacter = UpdateCharacter;
            GD.Print("Movement Character: ", CurrentCharacter.ToString());
        }

        public void DisplayOptionsOnGrid()
        {
            switch (displayState.currentState)
            {
                case InteractionStates.DisplayMovement:
                    movementSystem.DisplayPotentialDestinations(CurrentCharacter);
                    break;
                case InteractionStates.DisplayWeaponDetails:
                    damageSystem.DisplayPotentialWeaponTargets(CurrentCharacter);
                    break;
                default:
                    movementSystem.DisplayPotentialDestinations(CurrentCharacter);
                    break;
            }
        }

        public void HideOptionsOnGrid()
        {
            switch (displayState.currentState)
            {
                case InteractionStates.DisplayMovement:
                    movementSystem.HidePotentialDestinations(CurrentCharacter);
                    break;
                case InteractionStates.DisplayWeaponDetails:
                    damageSystem.DisplayPotentialWeaponTargets(CurrentCharacter);
                    break;
                default:
                    movementSystem.HidePotentialDestinations(CurrentCharacter);
                    break;
            }
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

        public void ProcessUnitOrder(Node tile_collider)
        {
            GD.Print("Processing Right Click");
            TargetableEntity target = null;

            string tile_name = tile_collider.Name;
            int divider = tile_name.Find(",");
            int q = tile_name.Substring(4, divider - 4).ToInt();
            int r = tile_name.Substring(divider + 1).ToInt();
            Vector2I DesiredTileLocation = new Vector2I(q, r);
            if (TileGrid.HasMethod("FindTileOnGrid"))
            {
                Variant variantTileResource = TileGrid.Call("FindTileOnGrid", DesiredTileLocation);
                GodotObject tileResource = variantTileResource.AsGodotObject();
                if (tileResource.HasMethod("GetCharacterOnTile"))
                {
                    Variant variantCharacter = tileResource.Call("GetCharacterOnTile");
                    if (variantCharacter.AsGodotObject() != null)
                    {
                        target = variantCharacter.AsGodotObject() as TargetableEntity;
                    }
                }
            }

            //If there is a targetable entity, send it to the DamageSystem
            if (target != null)
            {
                Node tile = tile_collider.GetParent();
                Vector2I SourceCharacterLocation = new Vector2I(0, 0);
                if (tile.HasMethod("GetCoordinateFromPosition"))
                {
                    SourceCharacterLocation = (Vector2I)tile.Call("GetCoordinateFromPosition", CurrentCharacter.Position, 3.0f);
                }

                damageSystem.AttackTile(DesiredTileLocation, SourceCharacterLocation, CurrentCharacter);
            }
            //Otherwise, attempt to move to the tile
            else
            {
                movementSystem.MoveCharacter(CurrentCharacter, tile_collider);
            }
        }


        public void ResetTileAfterCharacterDeath(BaseCharacter character)
        {
            Vector2I character_location = (Vector2I)TileGrid.Call("GetCoordinateFromPosition", character.Position, 3.0f);
            if (TileGrid.HasMethod("FindTileOnGrid"))
            {
                GodotObject character_tile = TileGrid.Call("FindTileOnGrid", character_location).AsGodotObject();
                if (character_tile.HasMethod("ResetCharacterOnTile"))
                {
                    character_tile.Call("ResetCharacterOnTile");
                }
            }


        }

    }
}
