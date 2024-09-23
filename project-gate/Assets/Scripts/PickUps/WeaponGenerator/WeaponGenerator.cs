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
    generatedWeapon.SetWeaponDamage((int)parsedData["Damage"]);
    generatedWeapon.SetWeaponName((string)parsedData["Name"]);
    generatedWeapon.SetMaxRange((int)parsedData["Range"]);
    string onhitbehavior = (string)parsedData["OnHitBehavior"];
    GD.Print(WEAPON_BEHAVIOR_DIRECTORY, onhitbehavior, ".cs");
    CSharpScript behavior = ResourceLoader.Load(WEAPON_BEHAVIOR_DIRECTORY + onhitbehavior + ".cs") as CSharpScript;
    generatedWeapon.SetOnHitBehavior((GodotObject)behavior.New() as IOnHitBehavior);

    return generatedWeapon;
  }

  public Grenade GenerateGrenade(string grenadePath) {
    if(grenadePath == "none") {
      return null;
    }

    Grenade generatedGrenade = new Grenade();
    return generatedGrenade;
  }

}
