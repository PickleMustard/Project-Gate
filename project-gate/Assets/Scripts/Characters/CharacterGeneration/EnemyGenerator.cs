using Godot;
using Godot.Collections;
using System.Text.RegularExpressions;
using ProjGate.Pickups;

namespace ProjGate.Character
{
  public partial class EnemyGenerator : Resource
  {
    private const string CONFIGURATION_BASE_PATH = "res://Configuration/";
    private const string ENEMY_CONFIGURATION_PATH = "res://Configuration/Enemies/";

    private GodotObject RNG;
    private WeaponGenerator weaponGenerator;
    private Regex matchPath = new Regex("^(.+)/([^/]+)$");

    public EnemyGenerator()
    {
      RNG = Engine.GetSingleton("GlobalSeededRandom");
      weaponGenerator = new WeaponGenerator();
    }

    public BaseCharacter GenerateEnemy(string configurationPath)
    {
      Enemy enemy = new Enemy();
      GodotObject yamlParser = ClassDB.Instantiate("YamlParser").AsGodotObject();
      string toLoad = ENEMY_CONFIGURATION_PATH + configurationPath + ".yml";
      Dictionary parsedData = (Dictionary)yamlParser.Call("parse_file", toLoad);

      string characterName = (string)parsedData["Name"];
      string EnemyIconLocation = (string)parsedData["Icon"];
      Texture2D EnemyIcon;
      if (matchPath.Match(EnemyIconLocation).Success) //If the path is an absolute path, use it
      {
        EnemyIcon = ResourceLoader.Load(EnemyIconLocation) as Texture2D;
      }
      else
      {
        toLoad = ENEMY_CONFIGURATION_PATH + "EnemyIcons/" + EnemyIconLocation + ".png";
        EnemyIcon = ResourceLoader.Load(toLoad) as Texture2D;
      }

      string chosenWeapon = (string)parsedData["Enemy-Weapon"];
      BaseWeapon startingWeapon = weaponGenerator.GenerateWeapon(chosenWeapon);

      string chosenGrenade = (string)parsedData["Enemy-Grenade"];
      BaseGrenade startingGrenade = null;
      if (chosenGrenade != "None")
      {
        startingGrenade = weaponGenerator.GenerateGrenade(chosenGrenade);
      }

      Array<BaseCharacter.WEAPON_PROFICIENCIES> proficiencies = new Array<BaseCharacter.WEAPON_PROFICIENCIES>();
      proficiencies.Resize(System.Enum.GetNames(typeof(BaseWeapon.WEAPON_TYPES)).Length);
      for (int i = 0; i < proficiencies.Count; i++)
      {
        proficiencies[i] = BaseCharacter.WEAPON_PROFICIENCIES.clumsy;
      }
      proficiencies[(int)startingWeapon.weaponType] = BaseCharacter.WEAPON_PROFICIENCIES.skilled;
      int movementRange = (int)parsedData["Movement-Range"];
      int actionPoints = (int)parsedData["Action-Points"];
      int turnPriority = (int)parsedData["Turn-Priority"];
      int characterHealth = (int)parsedData["Health"];
      int CurrencyDroppedOnDeath = (int)parsedData["Currency-Drop-Rate"];
      float requeueSpeed = (float)parsedData["Requeue-Speed"];
      float accumulationRate = (float)parsedData["Accumulation-Rate"];

      Node informationProvider = (Node)ClassDB.Instantiate((string)parsedData["IGOAP"]);
      enemy.AddChild(informationProvider, true);
      informationProvider.Name = "data_provider";

      Node agent = (Node)ClassDB.Instantiate("GoapAgent");
      enemy.AddChild(agent, true);
      agent.Name = "agent";

      Godot.Collections.Array Actions = (Godot.Collections.Array)parsedData["AI-Behaviors"];

      Godot.Collections.Dictionary availableActions = new Godot.Collections.Dictionary();
      for (int i = 0; i < Actions.Count; i++)
      {
        Resource action = (Resource)ClassDB.Instantiate((string)Actions[i]);
        availableActions.Add(action.Call("GetActionName"), action);
        GD.Print(availableActions);
      }
      if (agent.HasMethod("AddAvailableAction"))
      {
        GD.Print("Setting Available Actions");
        agent.Call("AddAvailableAction", availableActions);
      }

      GD.Print(ClassDB.ClassExists("AttackEnemyAction"));
      Resource behavior = (Resource)ClassDB.Instantiate("AttackEnemyAction");

      enemy.GenerateCharacter(characterName, startingWeapon, startingGrenade, proficiencies, movementRange, actionPoints, characterHealth, accumulationRate, requeueSpeed, turnPriority, CurrencyDroppedOnDeath, EnemyIcon);
      return enemy;

    }


  }
}
