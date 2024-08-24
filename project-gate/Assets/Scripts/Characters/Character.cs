using Godot;

public partial class Character : Node3D
{
  [Signal]
  public delegate void UpdateMainCharacterEventHandler();

  [Export]
  public int TotalDistance = 8;

  [Export]
  public int TotalHealth = 10;
  public int currentHealth {get; private set;}

  [Export]
  public Weapon main_weapon {get; private set;}
  [Export]
  public Grenade grenade {get; private set;}
  private Godot.Collections.Array items;

  private int distanceRemaining {get; set;}

  public bool isMoving {get; set;} = false;

  public override void _Ready()
  {
    distanceRemaining = TotalDistance;
    InputHandler i_handle = GetNode<Node>("/root/Top/input_handler") as InputHandler;
    i_handle.UpdateCharacter += MakeMainCharacter;
    i_handle.ResetCharacter += ResetDistanceRemaining;
    Node3D UnitMovement = GetNodeOrNull<Node3D>("/root/Top/pivot/UnitControl");
    if (UnitMovement.HasMethod("GetUpdateCharacterSignal"))
    {
      Connect(SignalName.UpdateMainCharacter, (Callable)UnitMovement.Call("GetUpdateCharacterSignal"));
    }
    Connect(SignalName.UpdateMainCharacter, GenerationCommunicatorSingleton.Instance.GetUpdateCharacterSignal());
    EmitSignal(SignalName.UpdateMainCharacter, this);
    currentHealth = TotalHealth;
    if(main_weapon == null) {
      main_weapon = new Weapon();
      main_weapon.SetWeaponName("Pistol");
      main_weapon.SetMaxRange(10);
    }
  }

  public void AttackCharacter(int damageAmount) {
    GD.Print("Current Health ", currentHealth);
    currentHealth -= damageAmount;
    if(currentHealth <= 0) {
      Visible = false;
    }
  }

  public void HealCharacter(int healAmount) {
    GD.Print("Healing ", healAmount);
    currentHealth += healAmount;
  }


  public void MakeMainCharacter()
  {
    GD.Print("Emitting Signal");
    EmitSignal(SignalName.UpdateMainCharacter, this);
  }

  public int GetDistanceRemaining() {
    return distanceRemaining;
  }

  public void DecrementDistanceRemaining(int decrementor) {
    distanceRemaining -= decrementor;
  }

  public void SetMainWeapon(Weapon UpdatedWeapon) {
    main_weapon = UpdatedWeapon;
  }

  public Weapon GetMainWeapon() {
    return main_weapon;
  }

  public void ResetDistanceRemaining() {
    GD.Print("Resetting Distance");
    distanceRemaining = TotalDistance;
  }

}
