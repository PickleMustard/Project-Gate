using Godot;

[GlobalClass]
public partial class Character : Node3D
{
  private const float INTITIAL_REQUEUE_PRIORITY = 1.0f;
  public enum CHARACTER_TEAM
  {
    player,
    enemy
  }

  [Signal]
  public delegate void UpdateMainCharacterEventHandler();
  [Signal]
  public delegate void UpdatedMovementRemainingEventHandler();
  [Signal]
  public delegate void UpdatedHeapPriorityEventHandler();

  [Export]
  public int TotalDistance = 8;
  [Export]
  public int TotalHealth = 10;
  [Export]
  public float BaseSpeedAccumulator = 2.0f;
  [Export]
  public float SpeedNeededToRequeue = 2.0f;
  [Export]
  public float StartingSpeed = 0.2f;
  [Export]
  public float HeapPriority = 1.0f;
  [Export]
  public Weapon MainWeapon { get; private set; }
  [Export]
  public Grenade grenade { get; private set; }
  [Export]
  public CHARACTER_TEAM team;

  public int currentHealth { get; private set; }
  public float CurrentHeapPriority { get; private set; }
  public bool isMoving { get; set; } = false;
  private float CurrentSpeed;
  private Godot.Collections.Array items;
  private Node TileGrid;
  private int distanceRemaining { get; set; }
  private Callable updateMovementCalcs;

  public string testWeaponGenerationName = "testweapon";

  public override void _Ready()
  {
    SetupCharacter();
    GD.Print("Character Setup");
    GD.Print(ToString());
    GD.Print(GetClass());
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

    if (MainWeapon == null)
    {
      //GenerationCommunicatorSingleton generator = Engine.GetSingleton("GenerationCommunicatorSingleton") as GenerationCommunicatorSingleton;
      //generator.GenerateItem(4);
//      MainWeapon = new Weapon();
//      MainWeapon.SetWeaponName("Pistol");
//      MainWeapon.SetMaxRange(10);
//      MainWeapon.SetWeaponDamage(2);
//      MainWeapon.SetOnHitBehavior(new SimpleDamageBehavior());

      WeaponGenerator weaponGenerator = new WeaponGenerator();
      MainWeapon = weaponGenerator.GenerateWeapon(testWeaponGenerationName);
    }
    GD.Print("Character");
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
    MainWeapon.OnHit(target);
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
    return this.GetType() + " character\nDistance Remaining: " + distanceRemaining + "\nCurrent Priority: " + CurrentHeapPriority;
  }

  public void DecrementDistanceRemaining(int decrementor)
  {
    distanceRemaining -= decrementor;
    EmitSignal(SignalName.UpdatedMovementRemaining, GetDistanceRemaining());
  }

  public bool UpdateMovementCalcs()
  {
    bool requeue = false;
    GD.Print("CurrentHeapPriority: ", CurrentHeapPriority);
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
