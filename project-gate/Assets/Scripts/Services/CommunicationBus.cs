using Godot;
using ProjGate.Pickups;

public partial class CommunicationBus : Node
{
  [Signal]
  public delegate void IdentifyStrayNodeEventHandler();

  [Signal]
  public delegate void AddCurrencyEventHandler();

  [Signal]
  public delegate void DecementCurrencyEventHandler();


  public static CommunicationBus Instance { get; private set; }

  public Character CurrentCharacter { get; set; }

  private Callable SpawnEnemyCall;
  private Callable SpawnCharacterCall;
  private Callable UpdateCharacterCall;
  private Callable GenerateItemCall;
  public Callable CharacterKilledCall;

  private Node TileGrid;
  private Node3D Level;

  public override void _EnterTree()
  {
    Instance = this;
    Engine.RegisterSingleton("CommunicationBus", this);
    UpdateCharacterCall = new Callable(this, "UpdateCurrentCharacter");
    GenerateItemCall = new Callable(this, "GenerateItem");
    SpawnEnemyCall = new Callable(this, "SpawnEnemy");
    SpawnCharacterCall = new Callable(this, "SpawnPlayerCharacter");
    CharacterKilledCall = new Callable(this, "CharacterKilled");
  }


  public override void _Ready()
  {
    Connect(SignalName.AddCurrency, CurrencyService.Instance.GetIncrementCurrencyCallable());
    Connect(SignalName.DecementCurrency, CurrencyService.Instance.GetDecrementCurrencyCallable());
    Level = GetNodeOrNull<Node3D>("/root/Level");
  }

  public Callable GetGenerateItemSignal()
  {
    return GenerateItemCall;
  }
  public void GenerateItem(int type)
  {
    WeaponGenerator weaponGenerator = new WeaponGenerator();
    weaponGenerator.GenerateWeapon("testweapon");
    BaseWeapon weapon = new BaseWeapon();
    weapon.SetWeaponName("Ooga Booga Gun");
    CurrentCharacter.SetMainWeapon(weapon);
  }

  public Callable GetSpawnCharacterSignal()
  {
    return SpawnCharacterCall;
  }
  public Callable GetSpawnEnemySignal()
  {
    return SpawnEnemyCall;
  }
  public Callable GetCharacterKilledEventCallable()
  {
    return CharacterKilledCall;
  }

  public void AddCharacter(Character character, GenericCharacterBanner CharacterBanner)
  {
    Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
    MarginContainer bannerMargin = new MarginContainer();
    bannerMargin.SizeFlagsStretchRatio = 2;
    bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;
    bannerMargin.AddChild(CharacterBanner, true);
    charInf.AddChild(bannerMargin, true);
    GD.Print("Character Name: ", character.CharacterName);
    GD.Print("OOOOGA GOOGOASDFAL:KJDFG");
    CharacterBanner.UpdateCharacterName(character.CharacterName);
    CharacterBanner.UpdateIconBanner(character.GetCharacterIcon());
    CharacterBanner.UpdateMovementRemaining(character.GetDistanceRemaining());
    CharacterBanner.UpdateHeapPriority(character.HeapPriority);
    CharacterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
  }

  public void SpawnCharacter(Resource Tile, Character generatedCharacter, Node resourceProvider, Godot.Collections.Array<Resource> Actions)
  {
    Node character = ResourceLoader.Load<PackedScene>("res://Assets/Units/character.tscn").Instantiate();
    GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
    WeaponGenerator weaponGenerator = new WeaponGenerator();
    BaseWeapon startingWeapon = weaponGenerator.GenerateWeapon("res://Configuration/Weapons/testweapon.yml");
    Level.AddChild(character, true);
    Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
    MarginContainer bannerMargin = new MarginContainer();
    bannerMargin.AddChild(characterBanner, true);
    charInf.AddChild(bannerMargin, true);
    bannerMargin.SizeFlagsStretchRatio = 2;
    bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;

    characterBanner.UpdateCharacterName("This is a temporary name");
    characterBanner.UpdateMovementRemaining(CurrentCharacter.GetDistanceRemaining());
    characterBanner.UpdateHeapPriority(CurrentCharacter.HeapPriority);
    characterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
    CurrentCharacter.Connect(CurrentCharacter.GetSignalList()[1]["name"].ToString(), characterBanner.GetUpdateMovementCallable());
    CurrentCharacter.Connect(CurrentCharacter.GetSignalList()[2]["name"].ToString(), characterBanner.GetUpdateHeapPriorityCallable());
    character.Call("SetPosition", Tile);
  }



  public void SpawnPlayerCharacter(Resource Tile, Character generatedCharacter)
  {
    Node3D character = ResourceLoader.Load<PackedScene>("res://Assets/Units/playerteamcharacter.tscn").Instantiate() as Node3D;
    GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
    Level.AddChild(character, true);

    string name = character.Name;
    character.ReplaceBy(generatedCharacter, true);
    character.QueueFree();
    generatedCharacter.Name = name;
    Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
    MarginContainer bannerMargin = new MarginContainer();
    bannerMargin.AddChild(characterBanner, true);
    charInf.AddChild(bannerMargin, true);
    bannerMargin.SizeFlagsStretchRatio = 2;
    bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;

    characterBanner.UpdateCharacterName(generatedCharacter.CharacterName);
    characterBanner.UpdateIconBanner(generatedCharacter.GetCharacterIcon());
    characterBanner.UpdateMovementRemaining(generatedCharacter.GetDistanceRemaining());
    characterBanner.UpdateHeapPriority(generatedCharacter.HeapPriority);
    characterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
    generatedCharacter.Connect(generatedCharacter.GetSignalList()[1]["name"].ToString(), characterBanner.GetUpdateMovementCallable());
    generatedCharacter.Connect(generatedCharacter.GetSignalList()[2]["name"].ToString(), characterBanner.GetUpdateHeapPriorityCallable());
    generatedCharacter.AddToGroup("PlayerTeam");
    generatedCharacter.SetupCharacter();
    generatedCharacter.Call("SetPosition", Tile);
  }

  public void SpawnEnemy(Resource Tile, Character generatedCharacter)
  {
    Character enemy = ResourceLoader.Load<PackedScene>("res://Assets/Units/enemy.tscn").Instantiate() as Character;
    Level.AddChild(enemy, true);
    enemy.ReplaceBy(generatedCharacter);
    generatedCharacter.AddToGroup("Enemies");
    generatedCharacter.Name = generatedCharacter.CharacterName;
    generatedCharacter.SetupCharacter();
    enemy.QueueFree();

    //Godot.Collections.Array<Node> children = generatedCharacter.GetChildren();
    //for (int i = 0; i < children.Count; i++)
    //{
    //children[i].Owner = generatedCharacter;
    //}

    generatedCharacter.Call("SetPosition", Tile);
  }
  public Callable GetUpdateCharacterSignal()
  {
    return UpdateCharacterCall;
  }

  public void UpdateCurrentCharacter(Character UpdateCharacter)
  {
    CurrentCharacter = UpdateCharacter;
  }

  private void CharacterKilled(Character character)
  {
    GD.Print("Enemy Has Been Killed: ", character.Name);
    if(character.GetCharacterTeam() == (int)Character.CHARACTER_TEAM.enemy) {
      Enemy enemy = character as Enemy;
      EmitSignal(SignalName.AddCurrency, enemy.GetAmountOfCurrencyDroppedOnKill());
    }
    CharacterTurnController.Instance.RemoveCharacterFromTurnController(character);
    CharacterTurnController.Instance.RemoveUpdateCharacterMovementCallable(character.GetUpdateMovementCalculation());
    (GetTree().GetNodesInGroup("UnitControl")[0] as UnitControl).ResetTileAfterCharacterDeath(character);
    CharacterTurnController.Instance.UpdateMovementQueue();
  }

}
