using Godot;

public partial class GenerationCommunicatorSingleton : Node
{
  [Signal]
  public delegate void IdentifyStrayNodeEventHandler();

  public static GenerationCommunicatorSingleton Instance { get; private set; }

  public Character CurrentCharacter { get; set; }

  private Callable SpawnEnemyCall;
  private Callable SpawnCharacterCall;
  private Callable UpdateCharacterCall;
  private Callable GenerateItemCall;

  private Node TileGrid;
  private Node3D Level;

  public override void _EnterTree()
  {
    Instance = this;
    Engine.RegisterSingleton("GenerationCommunicatorSingleton", this);
    UpdateCharacterCall = new Callable(this, "UpdateCurrentCharacter");
    GenerateItemCall = new Callable(this, "GenerateItem");
    SpawnEnemyCall = new Callable(this, "SpawnEnemy");
    SpawnCharacterCall = new Callable(this, "SpawnPlayerCharacter");
  }

  public override void _Ready()
  {
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
    Weapon weapon = new Weapon();
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
  public void SpawnCharacter(Resource Tile, Character generatedCharacter, Node resourceProvider, Godot.Collections.Array<Resource> Actions)
  {
    ResourceLoader.Load<Resource>("AttackEnemyAction");
    Node character = ResourceLoader.Load<PackedScene>("res://Assets/Units/character.tscn").Instantiate();
    GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
    WeaponGenerator weaponGenerator = new WeaponGenerator();
    Weapon startingWeapon = weaponGenerator.GenerateWeapon("res://Configuration/Weapons/testweapon.yml");
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
    generatedCharacter.Name = name;
    Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
    MarginContainer bannerMargin = new MarginContainer();
    bannerMargin.AddChild(characterBanner, true);
    charInf.AddChild(bannerMargin, true);
    bannerMargin.SizeFlagsStretchRatio = 2;
    bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;

    characterBanner.UpdateCharacterName(generatedCharacter.CharacterName);
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

}
