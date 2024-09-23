using Godot;
using Godot.Collections;

public partial class CharacterGenerator : Resource
{

  private const string CONFIGURATION_BASE_PATH = "res://Configuration/";
  private const string GENERATED_CHARACTER_BASE_PATH = "res://Configuration/Characters/";

  private GodotObject RNG;
  private WeaponGenerator weaponGenerator;

  public CharacterGenerator() {
    GD.Print("Setting up CharacterGenerator");
    RNG = Engine.GetSingleton("GlobalSeededRandom");
    weaponGenerator = new WeaponGenerator();
  }

  public Character GenerateCharacter(string characterConfiguration)
  {
    Character createdCharacter = new Character();
    GodotObject yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();
    string toLoad = GENERATED_CHARACTER_BASE_PATH + characterConfiguration + ".yml";
    Dictionary parsedData = (Dictionary)yamlParser.Call("parse_file", toLoad);
    GD.Print(parsedData);
    string potentialNamesPath = (string)parsedData["Potential-Names"];
    toLoad = GENERATED_CHARACTER_BASE_PATH + potentialNamesPath + ".yml";
    Dictionary potentialNames = (Dictionary)yamlParser.Call("parse_file", toLoad);
    string characterName = ChooseCharacterName((Array)potentialNames["Potential-Names"]);

    toLoad = CONFIGURATION_BASE_PATH + (string)parsedData["Potential-Starting-Weapons"] + ".yml";
    Dictionary potentialWeapons = (Dictionary)yamlParser.Call("parse_file", toLoad);
    string chosenWeapon = ChooseCharacterWeapon((Array)potentialWeapons["Potential-Weapons"]);
    Weapon startingWeapon = weaponGenerator.GenerateWeapon(chosenWeapon);
    GD.Print(startingWeapon);

    toLoad = CONFIGURATION_BASE_PATH + (string)parsedData["Potential-Starting-Grenades"] + ".yml";
    Dictionary potentialGrenades = (Dictionary)yamlParser.Call("parse_file", toLoad);
    string chosenGrenade = ChooseCharacterGrenade((Array)potentialGrenades["Potential-Grenades"]);
    Grenade startingGrenade = weaponGenerator.GenerateGrenade(chosenGrenade);
    GD.Print(startingGrenade);

    int turnPriority = ChooseTurnPriority((Array)parsedData["Turn-Priority-Bounds"]);
    int movementRange = ChooseMovementRange((Array)parsedData["Movement-Range-Bounds"]);
    int actionPoints = ChooseActionPoints((Array)parsedData["Action-Points-Bounds"]);
    float accumulationRate = ChooseAccumulationRate((Array)parsedData["Accumulation-Rate-Bounds"]);
    float requeueSpeed = ChooseRequeueSpeed((Array)parsedData["Requeue-Speed-Bounds"]);

    createdCharacter.GenerateCharacter(characterName, startingWeapon, startingGrenade, movementRange, actionPoints, accumulationRate, requeueSpeed, turnPriority);
    GD.Print(createdCharacter);
    GD.Print(parsedData["Potential-Proficiencies"]);

    return createdCharacter;
  }

  private Array<Character.WEAPON_PROFICIENCIES> ChooseProficiencies(Dictionary Proficiencies) {
    Array<Character.WEAPON_PROFICIENCIES> generatedProficiencies = new Array<Character.WEAPON_PROFICIENCIES>();
    generatedProficiencies.Resize(System.Enum.GetNames(typeof(Weapon.WEAPON_TYPES)).Length - 1);
    int numProficient = (int)Proficiencies[""];
    return generatedProficiencies;
  }

  private string ChooseCharacterName(Array names)
  {
    int num = (int)RNG.Call("GetWholeNumber", names.Count - 1);
    return (string)names[num];
  }

  private string ChooseCharacterWeapon(Array weapons) {
    int num = (int)RNG.Call("GetWholeNumber", weapons.Count - 1);
    return (string)weapons[num];
  }

  private string ChooseCharacterGrenade(Array grenades) {
    int shouldHaveGrenade = (int)RNG.Call("GetWholeNumber", 4);
    if(shouldHaveGrenade > 3) {
      int num = (int)RNG.Call("GetWholeNumber", grenades.Count - 1);
      return (string)grenades[num];
    }
    return "none";
  }

  private int ChooseMovementRange(Array movementRangeBounds) {
    int movementRange = (int)RNG.Call("GetInteger", (int)movementRangeBounds[0], (int)movementRangeBounds[1]);
    return movementRange;
  }

  private int ChooseActionPoints(Array actionPointBounds) {
    int actionPoints = (int)RNG.Call("GetInteger", (int)actionPointBounds[0], (int)actionPointBounds[1]);
    return actionPoints;
  }

  private float ChooseAccumulationRate(Array AccumRateBounds) {
    float accumRate = (float)RNG.Call("GetFloatRange", (float)AccumRateBounds[0], (float)AccumRateBounds[1]);
    return accumRate;
  }

  private float ChooseRequeueSpeed(Array RequeueSpeedBounds) {
    float requeueSpeed = (float)RNG.Call("GetFloatRange", (float)RequeueSpeedBounds[0], (float)RequeueSpeedBounds[1]);
    return requeueSpeed;
  }

  private int ChooseTurnPriority(Array TurnPriorityBounds) {
    int turnPriority = (int)RNG.Call("GetInteger", (int)TurnPriorityBounds[0], (int)TurnPriorityBounds[1]);
    return turnPriority;
  }
}
