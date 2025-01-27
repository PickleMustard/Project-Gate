using Godot;
using System;
using System.Collections.Generic;
using Godot.Collections;
using ProjGate.Pickups;

[GlobalClass]
public partial class Character : Node3D
{
  [Signal]
  public delegate void CharacterKilledEventHandler();

  protected const float INTITIAL_REQUEUE_PRIORITY = 1.0f;
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
  public string CharacterName { get; protected set; } = "Temp";
  [Export]
  public int TotalActionPoints { get; protected set; }
  [Export]
  public int TotalDistance { get; protected set; } = 5;
  [Export]
  public int TotalHealth { get; protected set; } = 5;
  [Export]
  public float BaseSpeedAccumulator = 1.0f;
  [Export]
  public float SpeedNeededToRequeue = 1.0f;
  [Export]
  public float StartingSpeed = 0.1f;
  [Export]
  public float HeapPriority = 1;
  [Export]
  public BaseWeapon MainWeapon { get; protected set; }
  [Export]
  public BaseGrenade grenade { get; protected set; }
  [Export]
  public CHARACTER_TEAM team;
  [Export]
  public Texture2D Icon;
  [Export]
  public int distanceRemaining { get; set; }
  [Export]
  public GenericAbility SecondaryAbility { get; private set; }
  [Export]
  public GenericAbility UtilityAbility { get; private set; }
  [Export]
  public GenericAbility MovementAbility { get; private set; }
  [Export]
  public GenericAbility UltimateAbility { get; private set; }
  public Array<WEAPON_PROFICIENCIES> proficiencies = new Array<WEAPON_PROFICIENCIES>();
  public List<IGenericPassiveBehavior> passiveAbilities = new List<IGenericPassiveBehavior>();

  protected float CurrentSpeed;
  protected int currentHealth;
  protected int currentActionPoints;
  protected float CurrentHeapPriority = 1.0f;
  public bool isMoving { get; set; } = false;
  protected Godot.Collections.Array items;
  protected Node TileGrid;
  private Callable updateMovementCalcs;

  public void GenerateCharacter(string name, BaseWeapon weapon, BaseGrenade grenade, Array<WEAPON_PROFICIENCIES> proficiencies,
      int movementDistance, int actionPoints, int health, float accumulationRate, float requeueSpeed,
      int turnPriority, Texture2D icon,
      GenericAbility secondary, GenericAbility utility,
      GenericAbility movement, GenericAbility ultimate)
  {
    this.CharacterName = name;
    this.MainWeapon = weapon;
    this.grenade = grenade;
    this.proficiencies = proficiencies;
    this.TotalDistance = movementDistance;
    this.TotalActionPoints = actionPoints;
    this.TotalHealth = health;
    this.BaseSpeedAccumulator = accumulationRate;
    this.SpeedNeededToRequeue = requeueSpeed;
    this.HeapPriority = turnPriority;
    this.Icon = icon;
    SecondaryAbility = secondary;
    UtilityAbility = utility;
    MovementAbility = movement;
    UltimateAbility = ultimate;
  }

  public void IdentifyStray()
  {
    if (!IsInsideTree())
    {
      GD.Print("I'm a stray node Character");
    }
  }

  public override void _Ready()
  {
    Connect(SignalName.CharacterKilled, CommunicationBus.Instance.GetCharacterKilledEventCallable());
    CommunicationBus s = (CommunicationBus)Engine.GetSingleton("CommunicationBus");
    s.IdentifyStrayNode += IdentifyStray;
  }

  public void SetupCharacter()
  {
    //Connect(CommunicationBus.SignalName.IdentifyStrayNode, new Callable(this, "IdentifyStray"));
    Node level = GetTree().GetNodesInGroup("Level")[0];
    TileGrid = level.GetChildren()[0];
    CurrentSpeed = StartingSpeed;
    distanceRemaining = TotalDistance;
    currentActionPoints = TotalActionPoints;
#if TOOLS
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
    Connect(SignalName.UpdateMainCharacter, CommunicationBus.Instance.GetUpdateCharacterSignal());
    EmitSignal(SignalName.UpdateMainCharacter, this);
    CharacterTurnController.Instance.AddUpdateCharacterMovementCallable(updateMovementCalcs);
    CharacterTurnController.Instance.AddCharacterToTurnController(this);

    SetupHealthbar();
    ResetPriority();
  }

  public Texture2D GetCharacterIcon()
  {
    return Icon;
  }
  public Callable GetUpdateMovementCalculation()
  {
    return updateMovementCalcs;
  }
  protected void SetupHealthbar()
  {
    currentHealth = TotalHealth;
    Healthbar hb = FindChild("Healthbar") as Healthbar;
    if (hb != null)
    {
      hb.SetupHealthBar(TotalHealth);
    }
  }
  protected void UpdateHealthbar()
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

  public void AttackCharacter(Resource target)
  {
    GD.Print("Attac Character Calculations: ", (int)MainWeapon.weaponType, " | ", proficiencies[(int)MainWeapon.weaponType], " | ", (int)proficiencies[(int)MainWeapon.weaponType]);
    PlayAttackSound();
    MainWeapon.OnHit(this, target, GetTree().GetNodesInGroup("Tilegrid")[0]);
    currentActionPoints--;
    EmitSignal(SignalName.OnHitPassiveBehavior);
  }

  public void PlayAttackSound()
  {
    AudioStreamPlayer3D player = (AudioStreamPlayer3D)FindChild("weapon_attack");
    player.Play();
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

  protected void KillCharacter()
  {
    GD.Print("I'm dying here");
    AudioStreamPlayer3D player = (AudioStreamPlayer3D)FindChild("character_death");
    player.Play();
    Visible = false;

    EmitSignal(SignalName.CharacterKilled, this);
    //QueueFree();
  }

  public void EndCharacterTurn()
  {
    EmitSignal(SignalName.EndTurnPassiveBehavior);
  }

  public void MovedCharacter()
  {
    EmitSignal(SignalName.StepOnTilePassiveBehavior);
  }

  public void AddItemToCharacter()
  {
    EmitSignal(SignalName.PickUpItemPassiveBehavior);
  }

  public void RemoveItemFromCharacter()
  {

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

  public int GetOptimalWeaponRange()
  {
    return MainWeapon.EffectiveRange;
  }

  public int GetCharacterTeam()
  {
    return (int)team;
  }

  public int GetActionPointsRemaining()
  {
    return currentActionPoints;
  }

  public float GetCurrentHeapPriority()
  {
    return CurrentHeapPriority;
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

  public (bool, bool) UpdateMovementCalculation()
  {
    bool enqueue = false;
    bool requeue = false;
    GD.Print("CurrentHeapPriority: ", CurrentHeapPriority, "| Should Requeue: ", requeue, "| CurrentSpeed: ", CurrentSpeed, "| Requeue Speed: ", SpeedNeededToRequeue);
    if (CurrentHeapPriority < 0)
    {
      CurrentSpeed += BaseSpeedAccumulator;
    }
    if (CurrentSpeed >= SpeedNeededToRequeue)
    {
      //Replace with signal
      //CharacterTurnController.Instance.AddCharacterToMovementQueue(this);
      enqueue = true;
      CurrentSpeed -= SpeedNeededToRequeue;
      if (CurrentSpeed >= SpeedNeededToRequeue)
      {
        requeue = true;
      }
    }
    return (enqueue, requeue);
  }

  public void SetMainWeapon(BaseWeapon UpdatedWeapon)
  {
    MainWeapon = UpdatedWeapon;
  }

  public BaseWeapon GetMainWeapon()
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
