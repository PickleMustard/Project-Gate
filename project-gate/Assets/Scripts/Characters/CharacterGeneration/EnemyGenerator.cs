using Godot;
using Godot.Collections;

public partial class EnemyGenerator : Resource
{
  private const string CONFIGURATION_BASE_PATH = "res://Configuration/";
  private const string ENEMY_CONFIGURATION_PATH = "res://Configuration/Enemies/";

  private GodotObject RNG;
  private WeaponGenerator weaponGenerator;

  public EnemyGenerator()
  {
    RNG = Engine.GetSingleton("GlobalSeededRandom");
    weaponGenerator = new WeaponGenerator();
  }

  public Character GenerateEnemy(string configurationPath)
  {
    Enemy enemy = new Enemy();
    GodotObject yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();
    string toLoad = ENEMY_CONFIGURATION_PATH + configurationPath + ".yml";
    Dictionary parsedData = (Dictionary)yamlParser.Call("parse_file", toLoad);

    string characterName = (string)parsedData["Name"];

    string chosenWeapon = (string)parsedData["Enemy-Weapon"];
    Weapon startingWeapon = weaponGenerator.GenerateWeapon(chosenWeapon);

    string chosenGrenade = (string)parsedData["Enemy-Grenade"];
    Grenade startingGrenade = null;
    if (chosenGrenade != "None")
    {
      startingGrenade = weaponGenerator.GenerateGrenade(chosenGrenade);
    }

    Array<Character.WEAPON_PROFICIENCIES> proficiencies = new Array<Character.WEAPON_PROFICIENCIES>();
    proficiencies.Resize(System.Enum.GetNames(typeof(Weapon.WEAPON_TYPES)).Length);
    for (int i = 0; i < proficiencies.Count; i++)
    {
      proficiencies[i] = Character.WEAPON_PROFICIENCIES.clumsy;
    }
    proficiencies[(int)startingWeapon.weaponType] = Character.WEAPON_PROFICIENCIES.skilled;
    int movementRange = (int)parsedData["Movement-Range"];
    int actionPoints = (int)parsedData["Action-Points"];
    int turnPriority = (int)parsedData["Turn-Priority"];
    int characterHealth = (int)parsedData["Health"];
    float requeueSpeed = (float)parsedData["Requeue-Speed"];
    float accumulationRate = (float)parsedData["Accumulation-Rate"];

    GD.Print(ClassDB.ClassExists("AttackEnemyAction"));
    Resource behavior = (Resource)ClassDB.Instantiate("AttackEnemyAction");

    enemy.GenerateCharacter(characterName, startingWeapon, startingGrenade, proficiencies, movementRange, actionPoints, characterHealth, accumulationRate, requeueSpeed, turnPriority);
    return enemy;

  }


}
