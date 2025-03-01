GDPC                 �                                                                         X   res://.godot/exported/133200997/export-877244ca9cbdb9b374e92a000bf2d416-test_shotgun.res0      "      �8&�/�;ˉG��H    X   res://.godot/exported/133200997/export-87c8c3613bd105f82faaac564e062b17-test_rifle.res              ��x
t(|��Rr5*�    X   res://.godot/exported/133200997/export-e3045393e569e028af7fad4b6a67a02a-test_pistol.res                �W�i\��/;I�'_��/        res://.godot/extension_list.cfg `v             Xl�7���:yD=�f(�    ,   res://.godot/global_script_class_cache.cfg  `T      �      �4еW���a�wd�       res://.godot/uid_cache.bin  �g      �      �/�#5�b@�s�4�.�    @   res://Assets/Configured-Assets/Weapons/test_pistol.tres.remap   S      h       ��"�b1�G��CG�o    <   res://Assets/Configured-Assets/Weapons/test_rifle.tres.remap�S      g       I�B�����(�|�	    @   res://Assets/Configured-Assets/Weapons/test_shotgun.tres.remap  �S      i       ���HK)��9΢�    (   res://Assets/Scripts/Daemon/Daemon.cs           �      `����r zt�gn�o    ,   res://Assets/Scripts/PickUps/BaseWeapon.cs         �      т;�tb8t��G�Z\    4   res://Assets/Scripts/Services/CommunicationBus.cs   �4             �}�j~�=캯�ٖ�    ,   res://addons/HCoroutines/CoroutineManager.cs`      �      �ٶߖZ]���s���       res://icon.svg  @W      N      ]��s�9^w/�����       res://project.binary�v      �      5fu���&I��    RSRC                 	   Resource            ��������   BaseWeapon                                                   resource_local_to_scene    resource_name    script    WeaponIndex    WeaponName 	   MaxRange    EffectiveRange    IgnoresLineSight    BaseDamage    weaponType    OnHitBehaviorLocation    StartTurnBehaviorLocation    EndTurnBehaviorLocation    TargetingBehaviorLocation       Script +   res://Assets/Scripts/PickUps/BaseWeapon.cs ��������   8   res://Assets/Configured-Assets/Weapons/test_pistol.tres #      	   Resource                                   test_pistol                                      	         
      E   res://Assets/Scripts/PickUps/WeaponBehaviors/SimpleDamageBehavior.cs                               RSRCusing Godot;

namespace ProjGate.Pickups
{
  [GlobalClass]
  public partial class BaseWeapon : Resource
  {
    public enum WEAPON_TYPES
    {
      melee,
      handgun,
      shotgun,
      carbine,
      rifle,
      longbarrel,
      special
    }
    [Export]
    public uint WeaponIndex { get; private set; }
    [Export]
    public string WeaponName { get; private set; }
    [Export]
    public int MaxRange { get; private set; }
    [Export]
    public int EffectiveRange { get; private set; }
    [Export]
    public bool IgnoresLineSight { get; private set; }
    [Export]
    public int BaseDamage { get; private set; }
    [Export(PropertyHint.Enum)]
    public WEAPON_TYPES weaponType { get; private set; }

    [Export(PropertyHint.File, "*.cs")]
    public string OnHitBehaviorLocation { get; set; }
    [Export(PropertyHint.File, "*.cs")]
    public string StartTurnBehaviorLocation { get; set; }
    [Export(PropertyHint.File, "*.cs")]
    public string EndTurnBehaviorLocation { get; set; }
    [Export(PropertyHint.File, "*.cs")]
    public string TargetingBehaviorLocation { get; set; }

    private IOnHitBehavior OnHitBehavior;
    private IStartTurnBehavior StartTurnBehavior;
    private IEndTurnBehavior EndTurnBehavior;
    private IGetTargetingBehavior TargetingBehavior;

    public override void _SetupLocalToScene()
    {
      CSharpScript onHitBehavior = ResourceLoader.Load(OnHitBehaviorLocation) as CSharpScript;
      GD.Print("Local Weapon Behavior: OnHitBehavior using as: ", onHitBehavior);
      IOnHitBehavior OnHitBehavior = onHitBehavior.New().AsGodotObject() as IOnHitBehavior;

    }

    public void SetInitialWeaponParameters(string name, int maxRange, int effectiveRange, int baseDamage, string weaponType, bool ignoresLOS, bool canRicochet, bool canPierce,
        IOnHitBehavior onhit, IStartTurnBehavior startTurn, IEndTurnBehavior endTurn, IGetTargetingBehavior targeting)
    {
      this.WeaponName = name;
      this.MaxRange = maxRange;
      this.EffectiveRange = effectiveRange;
      this.BaseDamage = baseDamage;
      this.weaponType = (WEAPON_TYPES)System.Enum.Parse(typeof(WEAPON_TYPES), weaponType);
      this.IgnoresLineSight = ignoresLOS;

      this.OnHitBehavior = onhit;
      GD.Print("Set OnHitBehavior: ", OnHitBehavior);
      this.StartTurnBehavior = startTurn;
      this.EndTurnBehavior = endTurn;
      this.TargetingBehavior = targeting;
    }

    public void SetWeaponDamage(int newDamage)
    {
      BaseDamage = newDamage;
    }
    public void SetOnHitBehavior(IOnHitBehavior newBehavior)
    {
      OnHitBehavior = newBehavior;
    }
    public void SetWeaponType(WEAPON_TYPES type)
    {
      weaponType = type;
    }
    public void SetEffectiveRange(int range)
    {
      EffectiveRange = range;
    }
    public void SetIgnoreLineOfSight(bool shouldIgnore)
    {
      IgnoresLineSight = shouldIgnore;
    }
    public void OnHit(Character attacker, Resource targetedLocation, Node TileGrid)
    {
      if (OnHitBehavior != null)
      {
        GD.Print("OnHitBehavior");
        OnHitBehavior.CalculateOnHit(this, attacker, targetedLocation, TileGrid);
      }
    }
    public void SetWeaponName(string name)
    {
      WeaponName = name;
    }

    public string GetWeaponName()
    {
      return WeaponName;
    }

    public int GetMaxRange()
    {
      return MaxRange;
    }

    public void SetMaxRange(int range)
    {
      MaxRange = range;
    }
    public override string ToString()
    {
      return "Weapon of name" + WeaponName;

    }

  }
}
          RSRC                 	   Resource            ��������   BaseWeapon                                                   resource_local_to_scene    resource_name    script    WeaponIndex    WeaponName 	   MaxRange    EffectiveRange    IgnoresLineSight    BaseDamage    weaponType    OnHitBehaviorLocation    StartTurnBehaviorLocation    EndTurnBehaviorLocation    TargetingBehaviorLocation       Script +   res://Assets/Scripts/PickUps/BaseWeapon.cs ��������   7   res://Assets/Configured-Assets/Weapons/test_rifle.tres "      	   Resource                                   test_rifle       
                               	         
      E   res://Assets/Scripts/PickUps/WeaponBehaviors/SimpleDamageBehavior.cs                               RSRC  RSRC                 	   Resource            ��������   BaseWeapon                                                   resource_local_to_scene    resource_name    script    WeaponIndex    WeaponName 	   MaxRange    EffectiveRange    IgnoresLineSight    BaseDamage    weaponType    OnHitBehaviorLocation    StartTurnBehaviorLocation    EndTurnBehaviorLocation    TargetingBehaviorLocation       Script +   res://Assets/Scripts/PickUps/BaseWeapon.cs ��������   9   res://Assets/Configured-Assets/Weapons/test_shotgun.tres $      	   Resource                                   test_shotgun                                   
   	         
      E   res://Assets/Scripts/PickUps/WeaponBehaviors/SimpleDamageBehavior.cs                               RSRC              namespace HCoroutines;

using System;
using System.Collections.Generic;
using Godot;

public partial class CoroutineManager : Node {
    public static CoroutineManager Instance { get; private set; }
    public float DeltaTime { get; private set; }
    public double DeltaTimeDouble { get; private set; }

    private bool _isIteratingActiveCoroutines;
    private readonly HashSet<CoroutineBase> _activeCoroutines = new();
    private readonly HashSet<CoroutineBase> _coroutinesToDeactivate = new();
    private readonly HashSet<CoroutineBase> _coroutinesToActivate = new();

    public void StartCoroutine(CoroutineBase coroutine) {
        coroutine.Manager = this;
        coroutine.OnEnter();
    }

    public void ActivateCoroutine(CoroutineBase coroutine) {
        if (_isIteratingActiveCoroutines) {
            _coroutinesToActivate.Add(coroutine);
            _coroutinesToDeactivate.Remove(coroutine);
        } else {
            _activeCoroutines.Add(coroutine);
        }
    }

    public void DeactivateCoroutine(CoroutineBase coroutine) {
        if (_isIteratingActiveCoroutines) {
            _coroutinesToDeactivate.Add(coroutine);
            _coroutinesToActivate.Remove(coroutine);
        } else {
            _activeCoroutines.Remove(coroutine);
        }
    }

    public override void _EnterTree() {
        Instance = this;
    }

    public override void _Process(double delta) {
        DeltaTimeDouble = delta;
        DeltaTime = (float)delta;

        _isIteratingActiveCoroutines = true;

        foreach (CoroutineBase coroutine in _activeCoroutines) {
            if (coroutine.IsAlive && coroutine.IsPlaying) {
                try {
                    coroutine.Update();
                } catch (Exception e) {
                    GD.PrintErr(e.ToString());
                }
            }
        }

        _isIteratingActiveCoroutines = false;

        foreach (CoroutineBase coroutine in _coroutinesToActivate) {
            _activeCoroutines.Add(coroutine);
        }
        _coroutinesToActivate.Clear();

        foreach (CoroutineBase coroutine in _coroutinesToDeactivate) {
            _activeCoroutines.Remove(coroutine);
        }
        _coroutinesToDeactivate.Clear();
    }
}
       using Godot;
using Godot.Collections;
using System.Collections.Generic;
using System;

public partial class Daemon : Node
{
  private Godot.Collections.Array EnemySpawnLocations;
  private Godot.Collections.Array PlayerTeamSpawnLocations;

  private CharacterGenerator generator;
  private EnemyGenerator e_generator;
  private CommunicationBus communicator;

  private Node Level; //Takes the level generated signal to start its background processes
  private Node Characters;

  public static Daemon Instance;

  [Signal]
  public delegate void StartTurnControllerEventHandler();

  public override void _EnterTree()
  {
    Instance = this;
    Engine.RegisterSingleton("Daemon", this);
  }
  public override void _Ready()
  {
    EnemySpawnLocations = new Godot.Collections.Array();
    PlayerTeamSpawnLocations = new Godot.Collections.Array();
    generator = new CharacterGenerator();
    e_generator = new EnemyGenerator();
    communicator = Engine.GetSingleton("CommunicationBus") as CommunicationBus;
    Array<Node> NodeGroups = GetTree().GetNodesInGroup("Level");
    if (NodeGroups.Count > 0)
    {
      Level = NodeGroups[0];
    }
    NodeGroups = GetTree().GetNodesInGroup("Characters");
    if (NodeGroups.Count > 0)
    {
      Characters = NodeGroups[0];
    }

    Level.Connect("LevelGenerated", new Callable(this, "LevelStartProcesses"));
  }

  public void SetEnemySpawnLocations(Godot.Collections.Array EnemySpawnLocations)
  {
    this.EnemySpawnLocations = EnemySpawnLocations;
  }

  public void SetPlayerTeamSpawnLocations(Godot.Collections.Array PlayerTeamSpawnLocations)
  {
    this.PlayerTeamSpawnLocations = PlayerTeamSpawnLocations;
  }

  public void AddEnemySpawnLocation(Resource newLocation)
  {
    EnemySpawnLocations.Add(newLocation);
  }

  public void AddPlayerSpawnLocation(Resource newLocation)
  {
    PlayerTeamSpawnLocations.Add(newLocation);
  }

  private void LevelStartProcesses()
  {
    GD.Print("The Daemon awakens");
    CallDeferred("GenerateStartingEnemies");
    CallDeferred("AddPlayerTeam");
    CallDeferred("LevelStartFinalization");
  }

  private void LevelStartFinalization()
  {
    EmitSignal(SignalName.StartTurnController);
  }

  private void GenerateStartingEnemies()
  {
    List<int> used_locations = new List<int>();
    GodotObject rnd = Engine.GetSingleton("GlobalSeededRandom");
    for (int i = 0; i < 10; i++)
    {
      int location = (int)rnd.Call("GetInteger", 0, EnemySpawnLocations.Count - 1);
      while (used_locations.Contains(location))
      {
        location = (int)rnd.Call("GetInteger", 0, EnemySpawnLocations.Count - 1);
      }
      GD.Print("Attemptin to spawn something at ", location);
      Character generatedEnemy = e_generator.GenerateEnemy("BasicRuffian");
      GD.Print("Generated Enemy: ", generatedEnemy);
      //communicator.SpawnEnemy((Resource)EnemySpawnLocations[location], generatedEnemy);
      SpawnEnemy((Resource)EnemySpawnLocations[location], generatedEnemy);
      used_locations.Add(location);
    }
  }

  public void SpawnPlayerCharacter(Resource Tile, Character generatedCharacter)
  {
    Character character = ResourceLoader.Load<PackedScene>("res://Assets/Units/playerteamcharacter.tscn").Instantiate() as Character;
    GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
    Characters.AddChild(character, true);

    string name = character.Name;
    character.ReplaceBy(generatedCharacter, true);
    character.QueueFree();
    generatedCharacter.Name = name;
    GD.Print("Character Name Inbetween: ", generatedCharacter.CharacterName);
    ((CommunicationBus)Engine.GetSingleton("CommunicationBus")).AddCharacter(generatedCharacter, characterBanner);
    generatedCharacter.Connect(Character.SignalName.UpdatedMovementRemaining, characterBanner.GetUpdateMovementCallable());
    generatedCharacter.Connect(Character.SignalName.UpdatedHeapPriority, characterBanner.GetUpdateHeapPriorityCallable());
    generatedCharacter.AddToGroup("PlayerTeam");
    generatedCharacter.SetupCharacter();
    generatedCharacter.Call("SetPosition", Tile);

  }

  public void SpawnEnemy(Resource Tile, Character generatedEnemy)
  {
    Character enemy = ResourceLoader.Load<PackedScene>("res://Assets/Units/enemy.tscn").Instantiate() as Character;
    Characters.AddChild(enemy, true);
    enemy.ReplaceBy(generatedEnemy);
    generatedEnemy.AddToGroup("Enemies");
    generatedEnemy.Name = generatedEnemy.CharacterName;
    generatedEnemy.SetupCharacter();
    enemy.QueueFree();
    generatedEnemy.Call("SetPosition", Tile);
  }
  private void AddPlayerTeam()
  {
    List<int> used_location = new List<int>();
    Character generatedCharacter = generator.GenerateCharacter("GeneratedCharacter0");
    GD.Print("Character Name Before: ", generatedCharacter.CharacterName);
    SpawnPlayerCharacter((Resource)PlayerTeamSpawnLocations[0], generatedCharacter);
    generatedCharacter = generator.GenerateCharacter("GeneratedCharacter0");
    SpawnPlayerCharacter((Resource)PlayerTeamSpawnLocations[1], generatedCharacter);
    generatedCharacter = generator.GenerateCharacter("GeneratedCharacter0");
    SpawnPlayerCharacter((Resource)PlayerTeamSpawnLocations[2], generatedCharacter);
  }
}
    using Godot;
using ProjGate.Pickups;

public partial class CommunicationBus : Node
{
  [Signal]
  public delegate void IdentifyStrayNodeEventHandler();

  [Signal]
  public delegate void AddCurrencyEventHandler();

  [Signal]
  public delegate void DecementCurrencyEventHandler();


  public static CommunicationBus Instance { get; private set; }

  public Character CurrentCharacter { get; set; }

  private Callable SpawnEnemyCall;
  private Callable SpawnCharacterCall;
  private Callable UpdateCharacterCall;
  private Callable GenerateItemCall;
  public Callable CharacterKilledCall;

  private Node TileGrid;
  private Node3D Level;

  public override void _EnterTree()
  {
    Instance = this;
    Engine.RegisterSingleton("CommunicationBus", this);
    UpdateCharacterCall = new Callable(this, "UpdateCurrentCharacter");
    GenerateItemCall = new Callable(this, "GenerateItem");
    SpawnEnemyCall = new Callable(this, "SpawnEnemy");
    SpawnCharacterCall = new Callable(this, "SpawnPlayerCharacter");
    CharacterKilledCall = new Callable(this, "CharacterKilled");
  }


  public override void _Ready()
  {
    Connect(SignalName.AddCurrency, CurrencyService.Instance.GetIncrementCurrencyCallable());
    Connect(SignalName.DecementCurrency, CurrencyService.Instance.GetDecrementCurrencyCallable());
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
    BaseWeapon weapon = new BaseWeapon();
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
  public Callable GetCharacterKilledEventCallable()
  {
    return CharacterKilledCall;
  }

  public void AddCharacter(Character character, GenericCharacterBanner CharacterBanner)
  {
    Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
    MarginContainer bannerMargin = new MarginContainer();
    bannerMargin.SizeFlagsStretchRatio = 2;
    bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;
    bannerMargin.AddChild(CharacterBanner, true);
    charInf.AddChild(bannerMargin, true);
    GD.Print("Character Name: ", character.CharacterName);
    GD.Print("OOOOGA GOOGOASDFAL:KJDFG");
    CharacterBanner.UpdateCharacterName(character.CharacterName);
    CharacterBanner.UpdateIconBanner(character.GetCharacterIcon());
    CharacterBanner.UpdateMovementRemaining(character.GetDistanceRemaining());
    CharacterBanner.UpdateHeapPriority(character.HeapPriority);
    CharacterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
  }

  public void SpawnCharacter(Resource Tile, Character generatedCharacter, Node resourceProvider, Godot.Collections.Array<Resource> Actions)
  {
    Node character = ResourceLoader.Load<PackedScene>("res://Assets/Units/character.tscn").Instantiate();
    GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
    WeaponGenerator weaponGenerator = new WeaponGenerator();
    BaseWeapon startingWeapon = weaponGenerator.GenerateWeapon("res://Configuration/Weapons/testweapon.yml");
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
    character.QueueFree();
    generatedCharacter.Name = name;
    Node charInf = GetTree().GetNodesInGroup("CharacterInfo")[0];
    MarginContainer bannerMargin = new MarginContainer();
    bannerMargin.AddChild(characterBanner, true);
    charInf.AddChild(bannerMargin, true);
    bannerMargin.SizeFlagsStretchRatio = 2;
    bannerMargin.SizeFlagsHorizontal = Control.SizeFlags.ExpandFill;

    characterBanner.UpdateCharacterName(generatedCharacter.CharacterName);
    characterBanner.UpdateIconBanner(generatedCharacter.GetCharacterIcon());
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

  private void CharacterKilled(Character character)
  {
    GD.Print("Enemy Has Been Killed: ", character.Name);
    if(character.GetCharacterTeam() == (int)Character.CHARACTER_TEAM.enemy) {
      Enemy enemy = character as Enemy;
      EmitSignal(SignalName.AddCurrency, enemy.GetAmountOfCurrencyDroppedOnKill());
    }
    CharacterTurnController.Instance.RemoveCharacterFromTurnController(character);
    CharacterTurnController.Instance.RemoveUpdateCharacterMovementCallable(character.GetUpdateMovementCalculation());
    (GetTree().GetNodesInGroup("UnitControl")[0] as UnitControl).ResetTileAfterCharacterDeath(character);
    CharacterTurnController.Instance.UpdateMovementQueue();
  }

}
[remap]

path="res://.godot/exported/133200997/export-e3045393e569e028af7fad4b6a67a02a-test_pistol.res"
        [remap]

path="res://.godot/exported/133200997/export-87c8c3613bd105f82faaac564e062b17-test_rifle.res"
         [remap]

path="res://.godot/exported/133200997/export-877244ca9cbdb9b374e92a000bf2d416-test_shotgun.res"
       list=Array[Dictionary]([{
"base": &"Resource",
"class": &"BaseGrenade",
"icon": "",
"language": &"C#",
"path": "res://Assets/Scripts/PickUps/BaseGrenade.cs"
}, {
"base": &"Resource",
"class": &"BaseItem",
"icon": "",
"language": &"C#",
"path": "res://Assets/Scripts/PickUps/BaseItem.cs"
}, {
"base": &"Resource",
"class": &"BaseWeapon",
"icon": "",
"language": &"C#",
"path": "res://Assets/Scripts/PickUps/BaseWeapon.cs"
}, {
"base": &"Node3D",
"class": &"Character",
"icon": "",
"language": &"C#",
"path": "res://Assets/Scripts/Characters/Character.cs"
}, {
"base": &"Resource",
"class": &"SimpleDamageBehavior",
"icon": "",
"language": &"C#",
"path": "res://Assets/Scripts/PickUps/WeaponBehaviors/SimpleDamageBehavior.cs"
}])
        <svg height="128" width="128" xmlns="http://www.w3.org/2000/svg"><g transform="translate(32 32)"><path d="m-16-32c-8.86 0-16 7.13-16 15.99v95.98c0 8.86 7.13 15.99 16 15.99h96c8.86 0 16-7.13 16-15.99v-95.98c0-8.85-7.14-15.99-16-15.99z" fill="#363d52"/><path d="m-16-32c-8.86 0-16 7.13-16 15.99v95.98c0 8.86 7.13 15.99 16 15.99h96c8.86 0 16-7.13 16-15.99v-95.98c0-8.85-7.14-15.99-16-15.99zm0 4h96c6.64 0 12 5.35 12 11.99v95.98c0 6.64-5.35 11.99-12 11.99h-96c-6.64 0-12-5.35-12-11.99v-95.98c0-6.64 5.36-11.99 12-11.99z" fill-opacity=".4"/></g><g stroke-width="9.92746" transform="matrix(.10073078 0 0 .10073078 12.425923 2.256365)"><path d="m0 0s-.325 1.994-.515 1.976l-36.182-3.491c-2.879-.278-5.115-2.574-5.317-5.459l-.994-14.247-27.992-1.997-1.904 12.912c-.424 2.872-2.932 5.037-5.835 5.037h-38.188c-2.902 0-5.41-2.165-5.834-5.037l-1.905-12.912-27.992 1.997-.994 14.247c-.202 2.886-2.438 5.182-5.317 5.46l-36.2 3.49c-.187.018-.324-1.978-.511-1.978l-.049-7.83 30.658-4.944 1.004-14.374c.203-2.91 2.551-5.263 5.463-5.472l38.551-2.75c.146-.01.29-.016.434-.016 2.897 0 5.401 2.166 5.825 5.038l1.959 13.286h28.005l1.959-13.286c.423-2.871 2.93-5.037 5.831-5.037.142 0 .284.005.423.015l38.556 2.75c2.911.209 5.26 2.562 5.463 5.472l1.003 14.374 30.645 4.966z" fill="#fff" transform="matrix(4.162611 0 0 -4.162611 919.24059 771.67186)"/><path d="m0 0v-47.514-6.035-5.492c.108-.001.216-.005.323-.015l36.196-3.49c1.896-.183 3.382-1.709 3.514-3.609l1.116-15.978 31.574-2.253 2.175 14.747c.282 1.912 1.922 3.329 3.856 3.329h38.188c1.933 0 3.573-1.417 3.855-3.329l2.175-14.747 31.575 2.253 1.115 15.978c.133 1.9 1.618 3.425 3.514 3.609l36.182 3.49c.107.01.214.014.322.015v4.711l.015.005v54.325c5.09692 6.4164715 9.92323 13.494208 13.621 19.449-5.651 9.62-12.575 18.217-19.976 26.182-6.864-3.455-13.531-7.369-19.828-11.534-3.151 3.132-6.7 5.694-10.186 8.372-3.425 2.751-7.285 4.768-10.946 7.118 1.09 8.117 1.629 16.108 1.846 24.448-9.446 4.754-19.519 7.906-29.708 10.17-4.068-6.837-7.788-14.241-11.028-21.479-3.842.642-7.702.88-11.567.926v.006c-.027 0-.052-.006-.075-.006-.024 0-.049.006-.073.006v-.006c-3.872-.046-7.729-.284-11.572-.926-3.238 7.238-6.956 14.642-11.03 21.479-10.184-2.264-20.258-5.416-29.703-10.17.216-8.34.755-16.331 1.848-24.448-3.668-2.35-7.523-4.367-10.949-7.118-3.481-2.678-7.036-5.24-10.188-8.372-6.297 4.165-12.962 8.079-19.828 11.534-7.401-7.965-14.321-16.562-19.974-26.182 4.4426579-6.973692 9.2079702-13.9828876 13.621-19.449z" fill="#478cbf" transform="matrix(4.162611 0 0 -4.162611 104.69892 525.90697)"/><path d="m0 0-1.121-16.063c-.135-1.936-1.675-3.477-3.611-3.616l-38.555-2.751c-.094-.007-.188-.01-.281-.01-1.916 0-3.569 1.406-3.852 3.33l-2.211 14.994h-31.459l-2.211-14.994c-.297-2.018-2.101-3.469-4.133-3.32l-38.555 2.751c-1.936.139-3.476 1.68-3.611 3.616l-1.121 16.063-32.547 3.138c.015-3.498.06-7.33.06-8.093 0-34.374 43.605-50.896 97.781-51.086h.066.067c54.176.19 97.766 16.712 97.766 51.086 0 .777.047 4.593.063 8.093z" fill="#478cbf" transform="matrix(4.162611 0 0 -4.162611 784.07144 817.24284)"/><path d="m0 0c0-12.052-9.765-21.815-21.813-21.815-12.042 0-21.81 9.763-21.81 21.815 0 12.044 9.768 21.802 21.81 21.802 12.048 0 21.813-9.758 21.813-21.802" fill="#fff" transform="matrix(4.162611 0 0 -4.162611 389.21484 625.67104)"/><path d="m0 0c0-7.994-6.479-14.473-14.479-14.473-7.996 0-14.479 6.479-14.479 14.473s6.483 14.479 14.479 14.479c8 0 14.479-6.485 14.479-14.479" fill="#414042" transform="matrix(4.162611 0 0 -4.162611 367.36686 631.05679)"/><path d="m0 0c-3.878 0-7.021 2.858-7.021 6.381v20.081c0 3.52 3.143 6.381 7.021 6.381s7.028-2.861 7.028-6.381v-20.081c0-3.523-3.15-6.381-7.028-6.381" fill="#fff" transform="matrix(4.162611 0 0 -4.162611 511.99336 724.73954)"/><path d="m0 0c0-12.052 9.765-21.815 21.815-21.815 12.041 0 21.808 9.763 21.808 21.815 0 12.044-9.767 21.802-21.808 21.802-12.05 0-21.815-9.758-21.815-21.802" fill="#fff" transform="matrix(4.162611 0 0 -4.162611 634.78706 625.67104)"/><path d="m0 0c0-7.994 6.477-14.473 14.471-14.473 8.002 0 14.479 6.479 14.479 14.473s-6.477 14.479-14.479 14.479c-7.994 0-14.471-6.485-14.471-14.479" fill="#414042" transform="matrix(4.162611 0 0 -4.162611 656.64056 631.05679)"/></g></svg>
  ?   ²&B$�.   res://addons/HCoroutines/CoroutineManager.tscn񜹻fwCj7   res://Assets/Configured-Assets/Weapons/test_pistol.tres�X��.cu6   res://Assets/Configured-Assets/Weapons/test_rifle.tres��P�`r�(8   res://Assets/Configured-Assets/Weapons/test_shotgun.tres�m�Ɍ��M+   res://Assets/Icons/UnitIcons/Unit_Icon1.png.�In�RR.$   res://Assets/Icons/End-Turn-Icon.png���
�y%   res://Assets/Icons/target_round_b.png�B����+*   res://Assets/Materials/RangeIndicator.tres�(ha/   res://Assets/Materials/selectable_material.tres�i����dq#   res://Assets/Materials/testing.tres�S��1�C2;   res://Assets/Materials/test_interactable_tile_material.tresrȝL47�
)   res://Assets/Materials/test_material.tres^�E0 � 7   res://Assets/Materials/test_obstacle_tile_material.tresڻ�S�\�&6   res://Assets/Materials/test_spawner_tile_material.tres�L����06   res://Assets/Materials/test_starter_tile_material.tres��S75��*.   res://Assets/Materials/test_tile_material.tres�~)��s3   res://Assets/Sound-Effects/Dev-Only/Death_Sound.mp3n�I�2   res://Assets/Sound-Effects/Dev-Only/Explosion3.mp3>�R��~�;   res://Assets/Sound-Effects/Dev-Only/Explosion3_modified.mp37���S:   res://Assets/Temporary-Assets/Character-Icons/elephant.pngo�7�6�>9   res://Assets/Temporary-Assets/Character-Icons/giraffe.png��+��Z7   res://Assets/Temporary-Assets/Character-Icons/hippo.png�-@�W�Q8   res://Assets/Temporary-Assets/Character-Icons/monkey.pngL����d�97   res://Assets/Temporary-Assets/Character-Icons/panda.png�8�:�z8   res://Assets/Temporary-Assets/Character-Icons/parrot.pngu��9�	[9   res://Assets/Temporary-Assets/Character-Icons/penguin.pngJޝQ���o5   res://Assets/Temporary-Assets/Character-Icons/pig.png���rv8   res://Assets/Temporary-Assets/Character-Icons/rabbit.png�xH�c7   res://Assets/Temporary-Assets/Character-Icons/snake.png�L�%�!?6   res://Assets/Temporary-Assets/Enemy-Icons/elephant.png��[0Z�"5   res://Assets/Temporary-Assets/Enemy-Icons/giraffe.png̴�?I3&3   res://Assets/Temporary-Assets/Enemy-Icons/hippo.png���g�(4   res://Assets/Temporary-Assets/Enemy-Icons/monkey.png������+3   res://Assets/Temporary-Assets/Enemy-Icons/panda.png`�DcFlD4   res://Assets/Temporary-Assets/Enemy-Icons/parrot.png����"& 5   res://Assets/Temporary-Assets/Enemy-Icons/penguin.png���A�^81   res://Assets/Temporary-Assets/Enemy-Icons/pig.png�s���a4   res://Assets/Temporary-Assets/Enemy-Icons/rabbit.png�k��n6W3   res://Assets/Temporary-Assets/Enemy-Icons/snake.png�N�mL��5   res://Assets/Textures/Shaders/PulsatingSelection.tresW� Wxg&   res://Assets/Textures/Sprites/Gasp.pngH����S{+   res://Assets/Textures/Sprites/test_icon.png��q���'D   res://Assets/Textures/User-Interface/barHorizontal_green_mid 200.png���;�<3B   res://Assets/Textures/User-Interface/barHorizontal_red_mid 200.pngd_��0�8E   res://Assets/Textures/User-Interface/barHorizontal_yellow_mid 200.png�8�5��.   res://Assets/Tile_Meshes/Mesh_0_30_30_1_1.tresL�8�9L2   res://Assets/Tile_Meshes/Mesh_0_30_30_1_33792.tres��S����p2   res://Assets/Tile_Meshes/Mesh_0_30_30_1_34304.tres��i�?f22   res://Assets/Tile_Meshes/Mesh_0_30_30_1_34816.tres焹�u�A{2   res://Assets/Tile_Meshes/Mesh_0_30_30_1_35840.tresRB\@Jw�s2   res://Assets/Tile_Meshes/Mesh_0_30_30_1_38912.tres�DHX   res://Assets/Units/enemy.tscn���$���++   res://Assets/Units/playerteamcharacter.tscn�Ne��{Y   res://Scenes/bootup_scene.tscns�����D   res://Scenes/test_level.tscnjV��>'9.   res://User-Interface/Themes/main_ui_theme.tresd"�h�qo4   res://User-Interface/character_turn_banner_icon.tscn[�_�G�fa2   res://User-Interface/generic_character_banner.tscnB��[	Fc(   res://User-Interface/user_interface.tscn�$�u���i   res://default_bus_layout.tres��a��O   res://hex_tile.tscn�{��^    res://icon.svg�&��en9   res://sphere.tres      res://bin/example.gdextension
  ECFG      _custom_features         dotnet     application/config/name         Project-Gate   application/run/main_scene(         res://Scenes/bootup_scene.tscn     application/config/features$   "         4.3    Forward Plus       application/config/icon         res://icon.svg     autoload/CoroutineManager8      -   *res://addons/HCoroutines/CoroutineManager.cs      autoload/Daemon0      &   *res://Assets/Scripts/Daemon/Daemon.cs     autoload/CommunicationBus<      2   *res://Assets/Scripts/Services/CommunicationBus.cs  "   display/window/size/viewport_width      �  #   display/window/size/viewport_height      8     dotnet/project/assembly_name         Gate Of Scalad  "   editor/version_control/plugin_name      	   GitPlugin   *   editor/version_control/autoload_on_startup            global_group/PlayerCharacters,      "   Player Characters to be controlled     global_group/CurrencyDisplay<      1   Holds the Text Box for displaying player currency      input/move_left�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device         	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode    @ 	   key_label             unicode           location          echo          script         input/move_right�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device         	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode    @ 	   key_label             unicode           location          echo          script         input/move_back�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device         	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode    @ 	   key_label             unicode           location          echo          script         input/move_forward�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode    @ 	   key_label             unicode           location          echo          script         input/rotate_left�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode   Q   	   key_label             unicode    q      location          echo          script         input/rotate_right�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode   E   	   key_label             unicode    e      location          echo          script         input/mouse_left�              deadzone      ?      events              InputEventMouseButton         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          button_mask           position              global_position               factor       �?   button_index         canceled          pressed           double_click          script         input/mouse_right�              deadzone      ?      events              InputEventMouseButton         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          button_mask           position              global_position               factor       �?   button_index         canceled          pressed           double_click          script         input/update_character�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode   T   	   key_label             unicode    t      location          echo          script         input/reset_character�              deadzone      ?      events              InputEventKey         resource_local_to_scene           resource_name             device     ����	   window_id             alt_pressed           shift_pressed             ctrl_pressed          meta_pressed          pressed           keycode           physical_keycode   Q   	   key_label             unicode    q      location          echo          script         input/mouse_right_up8               deadzone      ?      events         +   rendering/gl_compatibility/item_buffer_size       �  4   rendering/limits/global_shader_variables/buffer_size      @B             