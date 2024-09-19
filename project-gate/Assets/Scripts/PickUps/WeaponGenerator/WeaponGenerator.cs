using Godot;
using Godot.Collections;

public class WeaponGenerator
{
  private const string WEAPON_GENERATION_CONFIGURATION_DIRECTORY = "res://Configuration/Weapons/";
  private const string WEAPON_BEHAVIOR_DIRECTORY = "res://Assets/Scripts/PickUps/WeaponBehaviors/";
  public Weapon GenerateWeapon(string weapon)
  {
    Weapon generatedWeapon = new Weapon();
    GodotObject yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();
    GD.Print("Yaml Parse: ", yamlParser);
    string toLoad = WEAPON_GENERATION_CONFIGURATION_DIRECTORY + weapon + ".yml";
    GD.Print("Attempting to load weapon file: ", toLoad);
    Dictionary parsedData = (Dictionary)yamlParser.Call("parse_file", toLoad);
    GD.Print("Parsed data: ", parsedData);
    generatedWeapon.SetWeaponDamage((int)parsedData["Damage"]);
    generatedWeapon.SetWeaponName((string)parsedData["Name"]);
    generatedWeapon.SetMaxRange((int)parsedData["Range"]);
    string onhitbehavior = (string)parsedData["OnHitBehavior"];
    GD.Print(WEAPON_BEHAVIOR_DIRECTORY, onhitbehavior, ".cs");
    CSharpScript behavior = ResourceLoader.Load(WEAPON_BEHAVIOR_DIRECTORY + onhitbehavior + ".cs") as CSharpScript;
    //behavior.New();
    generatedWeapon.SetOnHitBehavior((GodotObject)behavior.New() as IOnHitBehavior);

    return generatedWeapon;
  }

}
