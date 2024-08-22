using Godot;

public partial class GenerationCommunicatorSingleton : Node
{
  public static GenerationCommunicatorSingleton Instance { get; private set; }

  public Character CurrentCharacter { get; set; }

  private Callable SpawnEnemyCall;
  private Callable SpawnCharacterCall;
  private Callable UpdateCharacterCall;
  private Callable GenerateItemCall;

  private Node TileGrid;

  public override void _EnterTree()
  {
    Instance = this;
    Engine.RegisterSingleton("GenerationCommunicatorSingleton", this);
    UpdateCharacterCall = new Callable(this, "UpdateCurrentCharacter");
    GenerateItemCall = new Callable(this, "GenerateItem");
    SpawnEnemyCall = new Callable(this, "SpawnEnemy");
    SpawnCharacterCall = new Callable(this, "SpawnCharacter");
  }

  public override void _Ready()
  {
    //Instance = this;
    //Engine.RegisterSingleton("GenerationCommunicatorSingleton", this);
    UpdateCharacterCall = new Callable(this, "UpdateCurrentCharacter");
    GenerateItemCall = new Callable(this, "GenerateItem");
    SpawnEnemyCall = new Callable(this, "SpawnEnemy");
    SpawnCharacterCall = new Callable(this, "SpawnCharacter");
  }

  public Callable GetGenerateItemSignal()
  {
    return GenerateItemCall;
  }
  public void GenerateItem(int type)
  {
    GD.Print("Generating Weapon", type);
    Weapon weapon = new Weapon();
    weapon.SetWeaponName("Ooga Booga Gun");
    CurrentCharacter.SetMainWeapon(weapon);
    GD.Print("Generated Weapon: ", CurrentCharacter.GetMainWeapon().ToString());
  }

  public Callable GetSpawnCharacterSignal()
  {
    return SpawnCharacterCall;
  }
  public Callable GetSpawnEnemySignal()
  {
    return SpawnEnemyCall;
  }
  public void SpawnCharacter(Resource Tile)
  {
    GD.Print("Spawning Character");
    Character new_character = new Character();
  }

  public void SpawnEnemy(Resource Tile)
  {
    GD.Print("Spawning Enemy");
    Enemy new_enemy = new Enemy();
  }
  public Callable GetUpdateCharacterSignal()
  {
    return UpdateCharacterCall;
  }

  public void UpdateCurrentCharacter(Character UpdateCharacter)
  {
    CurrentCharacter = UpdateCharacter;
    GD.Print("ItemGeneration character: ", CurrentCharacter.ToString());
  }

}
