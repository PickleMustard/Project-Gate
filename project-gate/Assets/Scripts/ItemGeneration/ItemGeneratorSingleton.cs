using Godot;

public partial class ItemGeneratorSingleton : Node
{
  public static ItemGeneratorSingleton Instance { get; private set; }

  public Character CurrentCharacter { get; set; }

  private Callable UpdateCharacter;
  private Callable GenerateItemCall;

  private Node TileGrid;

  public override void _Ready()
  {
    Instance = this;
    Engine.RegisterSingleton("ItemGeneratorSingleton", this);
    UpdateCharacter = new Callable(this, "UpdateCurrentCharacter");
    GenerateItemCall = new Callable(this, "GenerateItem");
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
  public Callable GetUpdateCharacterSignal()
  {
    return UpdateCharacter;
  }

  public void UpdateCurrentCharacter(Character UpdateCharacter)
  {
    CurrentCharacter = UpdateCharacter;
    GD.Print("ItemGeneration character: ", CurrentCharacter.ToString());
  }

}
