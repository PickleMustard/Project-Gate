using Godot;
using Godot.Collections;

public partial class WeaponGenerator : Resource
{
  private const string WEAPON_GENERATION_CONFIGURATION_DIRECTORY = "res://Configuration/Weapons/";
  private const string WEAPON_BEHAVIOR_DIRECTORY = "res://Assets/Scripts/PickUps/WeaponBehaviors/";

  public Weapon GenerateWeapon(string weaponPath)
  {
    Weapon generatedWeapon = new Weapon();
    GodotObject yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();
    string toLoad = WEAPON_GENERATION_CONFIGURATION_DIRECTORY + weaponPath + ".yml";
    Dictionary parsedData = (Dictionary)yamlParser.Call("parse_file", toLoad);

    string weaponName = (string)parsedData["Name"];
    int baseDamage = (int)parsedData["Base-Damage"];
    int maxRange = (int)parsedData["Max-Range"];
    int effectiveRange = (int)parsedData["Effective-Range"];
    string weaponType = (string)parsedData["Weapon-Type"];
    bool ignoresLOS = (bool)parsedData["Ignores-LOS"];
    bool canRicochet = (bool)parsedData["Can-Ricochet"];
    bool canPierce = (bool)parsedData["Can-Pierce"];

    string onHitBehaviorPath = (string)parsedData["On-Hit-Behavior"];
    string startTurnBehaviorPath = (string)parsedData["Start-Turn-Behavior"];
    string endTurnBehaviorPath = (string)parsedData["End-Turn-Behavior"];
    string targetingBehaviorPath = (string)parsedData["Targeting-Behavior"];

    CSharpScript onHitBehavior = ResourceLoader.Load(WEAPON_BEHAVIOR_DIRECTORY + onHitBehaviorPath + ".cs") as CSharpScript;
    GD.Print("OnHitBehavior using as: ", onHitBehavior);
    IOnHitBehavior test = onHitBehavior.New().AsGodotObject() as IOnHitBehavior;
    GD.Print("OnHitBehavior second using as: ", test.GetType());
    CSharpScript targetingBehavior = ResourceLoader.Load(WEAPON_BEHAVIOR_DIRECTORY + targetingBehaviorPath + ".cs") as CSharpScript;
    CSharpScript startTurnBehavior = null;
    CSharpScript endTurnBehavior = null;

    if (startTurnBehaviorPath != "none")
    {
      startTurnBehavior = ResourceLoader.Load(WEAPON_BEHAVIOR_DIRECTORY + startTurnBehaviorPath + ".cs") as CSharpScript;
    }
    if (endTurnBehaviorPath != "none")
    {
      endTurnBehavior = ResourceLoader.Load(WEAPON_BEHAVIOR_DIRECTORY + endTurnBehaviorPath + ".cs") as CSharpScript;
    }

    generatedWeapon.SetInitialWeaponParameters(weaponName, maxRange, effectiveRange, baseDamage, weaponType, ignoresLOS, canRicochet, canPierce,
        test, startTurnBehavior as IStartTurnBehavior, endTurnBehavior as IEndTurnBehavior, targetingBehavior as IGetTargetingBehavior);


    return generatedWeapon;
  }

  public Grenade GenerateGrenade(string grenadePath)
  {
    if (grenadePath == "none")
    {
      return null;
    }

    Grenade generatedGrenade = new Grenade();
    return generatedGrenade;
  }

}
