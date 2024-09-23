using Godot;
using System;
using System.Collections.Generic;
using Godot.Collections;

[GlobalClass]
public partial class Character : Node3D
{
  private const float INTITIAL_REQUEUE_PRIORITY = 1.0f;
  public enum CHARACTER_TEAM
  {
    player,
    enemy
  }
  public enum WEAPON_PROFICIENCIES
  {
    skilled = 1,
    passable,
    clumsy
  }

  [Signal]
  public delegate void UpdateMainCharacterEventHandler();
  [Signal]
  public delegate void UpdatedMovementRemainingEventHandler();
  [Signal]
  public delegate void UpdatedHeapPriorityEventHandler();

  [Signal]
  public delegate void OnHitPassiveBehaviorEventHandler();
  [Signal]
  public delegate void RecieveDamagePassiveBehaviorEventHandler();
  [Signal]
  public delegate void EndTurnPassiveBehaviorEventHandler();
  [Signal]
  public delegate void StepOnTilePassiveBehaviorEventHandler();
  [Signal]
  public delegate void PickUpItemPassiveBehaviorEventHandler();

  [Export]
  public int TotalDistance {get; private set;} = 5;
  [Export]
  public int TotalHealth {get; private set;} = 5;
  [Export]
  public float BaseSpeedAccumulator = 1.0f;
  [Export]
  public float SpeedNeededToRequeue = 1.0f;
  [Export]
  public float StartingSpeed = 0.1f;
  [Export]
  public float HeapPriority = 1;
  [Export]
  public Weapon MainWeapon { get; private set; }
  [Export]
  public Grenade grenade { get; private set; }
  [Export]
  public CHARACTER_TEAM team;
  public Array<WEAPON_PROFICIENCIES> proficiencies = new Array<WEAPON_PROFICIENCIES>();
  public List<IGenericPassiveBehavior> passiveAbilities = new List<IGenericPassiveBehavior>();

  public int currentHealth { get; private set; }
  public float CurrentHeapPriority { get; private set; } = 1.0f;
  public bool isMoving { get; set; } = false;
  private float CurrentSpeed;
  private Godot.Collections.Array items;
  private Node TileGrid;
  [Export]
  public int distanceRemaining { get; set; }
  private Callable updateMovementCalcs;

  public void GenerateCharacter(string name, Weapon weapon, Grenade grenade, int movementDistance, int actionPoints, float accumulationRate, float requeueSpeed, int turnPriority){
    this.Name = name;
    this.MainWeapon = weapon;
    this.grenade = grenade;
    this.TotalDistance = movementDistance;
    this.BaseSpeedAccumulator = accumulationRate;
    this.SpeedNeededToRequeue = requeueSpeed;
    this.HeapPriority = turnPriority;
  }

  public void SetupCharacter()
  {
    Node level = GetTree().GetNodesInGroup("Level")[0];
    TileGrid = level.GetChildren()[0];
    CurrentSpeed = StartingSpeed;
    distanceRemaining = TotalDistance;
#if DEBUG
    InputHandler i_handle = GetTree().GetNodesInGroup("InputHandler")[0] as InputHandler;
    i_handle.UpdateCharacter += MakeMainCharacter;
    i_handle.ResetCharacter += ResetDistanceRemaining;
    GD.Print("Debugging in Character Setup");
#endif
    UnitControl UnitMovement = GetTree().GetNodesInGroup("UnitControl")[0] as UnitControl;
    if (UnitMovement.HasMethod("GetUpdateCharacterSignal"))
    {
      Connect(SignalName.UpdateMainCharacter, (Callable)UnitMovement.Call("GetUpdateCharacterSignal"));
    }
    Connect(SignalName.UpdateMainCharacter, GenerationCommunicatorSingleton.Instance.GetUpdateCharacterSignal());
    EmitSignal(SignalName.UpdateMainCharacter, this);
    CharacterTurnController.Instance.AddUpdateCharacterMovementCallable(updateMovementCalcs);
    CharacterTurnController.Instance.AddCharacterToTurnController(this);

    SetupHealthbar();
    ResetPriority();
  }

  private void SetupHealthbar()
  {
    currentHealth = TotalHealth;
    Healthbar hb = FindChild("Healthbar") as Healthbar;
    if (hb != null)
    {
      hb.SetupHealthBar(TotalHealth);
    }
  }
  private void UpdateHealthbar()
  {
    Healthbar hb = FindChild("Healthbar") as Healthbar;
    if (hb != null)
    {
      hb.UpdateHealthBar(currentHealth);
    }

  }

  public void RecieveDamage(int damageAmount)
  {
    EmitSignal(SignalName.RecieveDamagePassiveBehavior);
    GD.Print("Current Health ", currentHealth);
    currentHealth -= damageAmount;
    UpdateHealthbar();
    if (currentHealth <= 0)
    {
      KillCharacter();
    }
  }

  public void AttackCharacter(Character target)
  {
    GD.Print("Attac Character Calculations: ", (int)MainWeapon.weaponType, " | ", proficiencies[(int)MainWeapon.weaponType], " | ", (int)proficiencies[(int)MainWeapon.weaponType]);
    MainWeapon.OnHit(1.0f / (int)proficiencies[(int)MainWeapon.weaponType], target);
    EmitSignal(SignalName.OnHitPassiveBehavior);
  }

  public void RequeueingCharacter()
  {
    if (CurrentHeapPriority < 0)
    {
      CurrentHeapPriority = INTITIAL_REQUEUE_PRIORITY;
    }
    else
    {
      CurrentHeapPriority += 1;
    }
  }

  public void SetPosition(Resource Tile)
  {
    Vector2I location = new Vector2I();
    if (Tile.HasMethod("GetLocation"))
    {
      location = (Vector2I)Tile.Call("GetLocation");
    }
    if (TileGrid.HasMethod("GetPositionForHexFromCoordinate"))
    {
      Position = (Vector3)TileGrid.Call("GetPositionForHexFromCoordinate", location, 3.0f, true) + new Vector3(0, 5, 0);
    }
    if (Tile.HasMethod("SetCharacterOnTile"))
    {
      Tile.Call("SetCharacterOnTile", this);
    }
  }

  public void ResetPriority()
  {
    CurrentHeapPriority = -HeapPriority;
  }

  private void KillCharacter()
  {
    CharacterTurnController.Instance.RemoveCharacterFromTurnController(this);
    CharacterTurnController.Instance.RemoveUpdateCharacterMovementCallable(updateMovementCalcs);
    AudioStreamPlayer3D player = (AudioStreamPlayer3D)GetChild(1);
    player.Play();
    Visible = false;

  }

  public void EndCharacterTurn() {
    EmitSignal(SignalName.EndTurnPassiveBehavior);
  }

  public void MovedCharacter() {
    EmitSignal(SignalName.StepOnTilePassiveBehavior);
  }

  public void AddItemToCharacter() {
    EmitSignal(SignalName.PickUpItemPassiveBehavior);
  }

  public void RemoveItemFromCharacter() {

  }

  public void HealCharacter(int healAmount)
  {
    currentHealth += healAmount;
    UpdateHealthbar();
  }


  public void MakeMainCharacter()
  {
    EmitSignal(SignalName.UpdateMainCharacter, this);
  }

  public int GetDistanceRemaining()
  {
    return distanceRemaining;
  }

  public bool GetIsMoving()
  {
    return isMoving;
  }

  public override string ToString()
  {
    return this.GetType() + " " + Name + "\nDistance Remaining: " + distanceRemaining + "\nCurrent Priority: " + CurrentHeapPriority;
  }

  public void DecrementDistanceRemaining(int decrementor)
  {
    distanceRemaining -= decrementor;
    EmitSignal(SignalName.UpdatedMovementRemaining, GetDistanceRemaining());
  }

  public bool UpdateMovementCalcs()
  {
    bool requeue = false;
    GD.Print("CurrentHeapPriority: ", CurrentHeapPriority, "| Should Requeue: ", requeue, "| CurrentSpeed: ", CurrentSpeed, "| Requeue Speed: ", SpeedNeededToRequeue);
    if (CurrentHeapPriority < 0)
    {
      CurrentSpeed += BaseSpeedAccumulator;
    }
    if (CurrentSpeed >= SpeedNeededToRequeue)
    {
      CharacterTurnController.Instance.AddCharacterToMovementQueue(this);
      CurrentSpeed -= SpeedNeededToRequeue;
      if (CurrentSpeed >= SpeedNeededToRequeue)
      {
        requeue = true;
      }
    }
    return requeue;
  }

  public void SetMainWeapon(Weapon UpdatedWeapon)
  {
    MainWeapon = UpdatedWeapon;
  }

  public Weapon GetMainWeapon()
  {
    return MainWeapon;
  }

  public void ResetDistanceRemaining()
  {
    GD.Print("Resetting Distance");
    distanceRemaining = TotalDistance;
    EmitSignal(SignalName.UpdatedMovementRemaining, GetDistanceRemaining());
  }

}
