using Godot;
using Godot.Collections;
using ProjGate.Pickups;

public partial class CharacterGenerator : Resource
{

  private const string CONFIGURATION_BASE_PATH = "res://Configuration/";
  private const string GENERATED_CHARACTER_BASE_PATH = "res://Configuration/Characters/";

  private GodotObject RNG;
  private WeaponGenerator weaponGenerator;

  public CharacterGenerator()
  {
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
    BaseWeapon startingWeapon = weaponGenerator.GenerateWeapon(chosenWeapon);
    GD.Print(startingWeapon);

    toLoad = CONFIGURATION_BASE_PATH + (string)parsedData["Potential-Starting-Grenades"] + ".yml";
    Dictionary potentialGrenades = (Dictionary)yamlParser.Call("parse_file", toLoad);
    string chosenGrenade = ChooseCharacterGrenade((Array)potentialGrenades["Potential-Grenades"]);
    BaseGrenade startingGrenade = weaponGenerator.GenerateGrenade(chosenGrenade);
    GD.Print(startingGrenade);

    string PotentialIconsPath = (string)parsedData["Potential-Icons"];
    toLoad = GENERATED_CHARACTER_BASE_PATH + PotentialIconsPath + ".yml";
    GD.Print(toLoad);
    Dictionary PotentialIcons = (Dictionary)yamlParser.Call("parse_file", toLoad);
    Texture2D CharacterIcon = ChooseCharacterIcon((Array)PotentialIcons["Potential-Icons"]);

    Array<Character.WEAPON_PROFICIENCIES> proficiencies = ChooseProficiencies((Array)parsedData["Potential-Proficiencies"]);

    int turnPriority = ChooseTurnPriority((Array)parsedData["Turn-Priority-Bounds"]);
    int movementRange = ChooseMovementRange((Array)parsedData["Movement-Range-Bounds"]);
    int actionPoints = ChooseActionPoints((Array)parsedData["Action-Points-Bounds"]);
    int characterHealth = ChooseCharacterHealth((Array)parsedData["Health-Range-Bounds"]);
    float requeueSpeed = ChooseRequeueSpeed((Array)parsedData["Requeue-Speed-Bounds"]);
    float accumulationRate = ChooseAccumulationRate((Array)parsedData["Accumulation-Rate-Bounds"], requeueSpeed);
    Dictionary Abilities = null;

    createdCharacter.GenerateCharacter(characterName, startingWeapon, startingGrenade, proficiencies, movementRange, actionPoints, characterHealth, accumulationRate, requeueSpeed, turnPriority, CharacterIcon, Abilities);
    GD.Print(createdCharacter);
    GD.Print(parsedData["Potential-Proficiencies"]);

    return createdCharacter;
  }

  private Array<Character.WEAPON_PROFICIENCIES> ChooseProficiencies(Array Proficiencies)
  {
    Array<Character.WEAPON_PROFICIENCIES> generatedProficiencies = new Array<Character.WEAPON_PROFICIENCIES>();
    Array<string> usedProficiencies = new Array<string>();
    generatedProficiencies.Resize(System.Enum.GetNames(typeof(BaseWeapon.WEAPON_TYPES)).Length);
    GD.Print(Proficiencies);
    Array numberOfEachProficiency = (Array)((Dictionary)(Proficiencies[0]))["Number-Proficiencies"];
    int proficientAmount = (int)((Dictionary)(numberOfEachProficiency[0]))["Proficient"];
    int passableAmount = (int)((Dictionary)(numberOfEachProficiency[1]))["Passable"];
    int clumsyAmount = (int)((Dictionary)(numberOfEachProficiency[2]))["Clumsy"];
    Array<string> possibleProficientWeapons = (Array<string>)((Dictionary)(Proficiencies[1]))["Proficient"];
    Array<string> possiblePassableWeapons = (Array<string>)((Dictionary)(Proficiencies[2]))["Passable"];
    Array<string> possibleClumsyWeapons = (Array<string>)((Dictionary)(Proficiencies[3]))["Clumsy"];
    GD.Print("Amounts of Each: ", proficientAmount, ", ", passableAmount, ", ", clumsyAmount);
    GD.Print(possibleProficientWeapons);
    GD.Print(possiblePassableWeapons);
    GD.Print(possibleClumsyWeapons);
    for (int i = 0; i < proficientAmount; i++)
    {
      int num = (int)RNG.Call("GetInteger", possibleProficientWeapons.Count - 1);
      BaseWeapon.WEAPON_TYPES type = (BaseWeapon.WEAPON_TYPES)System.Enum.Parse(typeof(BaseWeapon.WEAPON_TYPES), (string)possibleProficientWeapons[num]);
      generatedProficiencies[(int)type] = Character.WEAPON_PROFICIENCIES.skilled;
      if (possiblePassableWeapons.Contains(possibleProficientWeapons[num]))
      {
        possiblePassableWeapons.Remove(possibleProficientWeapons[num]);
      }
      if (possibleClumsyWeapons.Contains(possibleProficientWeapons[num]))
      {
        possibleClumsyWeapons.Remove(possibleProficientWeapons[num]);
      }
      possibleProficientWeapons.Remove(possibleProficientWeapons[num]);
    }
    GD.Print(generatedProficiencies);

    for (int i = 0; i < passableAmount; i++)
    {
      int num = (int)RNG.Call("GetInteger", possiblePassableWeapons.Count - 1);
      BaseWeapon.WEAPON_TYPES type = (BaseWeapon.WEAPON_TYPES)System.Enum.Parse(typeof(BaseWeapon.WEAPON_TYPES), (string)possiblePassableWeapons[num]);
      generatedProficiencies[(int)type] = Character.WEAPON_PROFICIENCIES.passable;
      if (possibleClumsyWeapons.Contains(possiblePassableWeapons[num]))
      {
        possibleClumsyWeapons.Remove(possiblePassableWeapons[num]);
      }
      possiblePassableWeapons.Remove(possiblePassableWeapons[num]);
    }

    for (int i = 0; i < possibleClumsyWeapons.Count; i++)
    {
      BaseWeapon.WEAPON_TYPES type = (BaseWeapon.WEAPON_TYPES)System.Enum.Parse(typeof(BaseWeapon.WEAPON_TYPES), (string)possibleClumsyWeapons[i]);
      generatedProficiencies[(int)type] = Character.WEAPON_PROFICIENCIES.clumsy;
    }
    return generatedProficiencies;
  }

  private string ChooseCharacterName(Array names)
  {
    int num = (int)RNG.Call("GetWholeNumber", names.Count - 1);
    return (string)names[num];
  }

  private Texture2D ChooseCharacterIcon(Array icons)
  {
    int num = (int)RNG.Call("GetWholeNumber", icons.Count - 1);
    Texture2D LoadedIcon = ResourceLoader.Load((string)icons[num]) as Texture2D;
    return LoadedIcon;
  }

  private string ChooseCharacterWeapon(Array weapons)
  {
    int num = (int)RNG.Call("GetWholeNumber", weapons.Count - 1);
    return (string)weapons[num];
  }

  private string ChooseCharacterGrenade(Array grenades)
  {
    int shouldHaveGrenade = (int)RNG.Call("GetWholeNumber", 4);
    if (shouldHaveGrenade > 3)
    {
      int num = (int)RNG.Call("GetWholeNumber", grenades.Count - 1);
      return (string)grenades[num];
    }
    return "none";
  }

  private int ChooseMovementRange(Array movementRangeBounds)
  {
    int movementRange = (int)RNG.Call("GetInteger", (int)movementRangeBounds[0], (int)movementRangeBounds[1]);
    return movementRange;
  }

  private int ChooseActionPoints(Array actionPointBounds)
  {
    int actionPoints = (int)RNG.Call("GetInteger", (int)actionPointBounds[0], (int)actionPointBounds[1]);
    return actionPoints;
  }

  //Accumulation Rate should at least allow the character to requeue on an end turn
  //Can generate a higher speed that builds up reallocation points to requeue eventually
  private float ChooseAccumulationRate(Array AccumRateBounds, float RequeueSpeed)
  {
    float accumRate = (float)RNG.Call("GetFloatRange", (float)AccumRateBounds[0], (float)AccumRateBounds[1]);
    return RequeueSpeed + accumRate;
  }

  private float ChooseRequeueSpeed(Array RequeueSpeedBounds)
  {
    float requeueSpeed = (float)RNG.Call("GetFloatRange", (float)RequeueSpeedBounds[0], (float)RequeueSpeedBounds[1]);
    return requeueSpeed;
  }

  private int ChooseTurnPriority(Array TurnPriorityBounds)
  {
    int turnPriority = (int)RNG.Call("GetInteger", (int)TurnPriorityBounds[0], (int)TurnPriorityBounds[1]);
    return turnPriority;
  }

  private int ChooseCharacterHealth(Array HealthRangeBounds)
  {
    int characterHealth = (int)RNG.Call("GetInteger", (int)HealthRangeBounds[0], (int)HealthRangeBounds[1]);
    return characterHealth;
  }
}
