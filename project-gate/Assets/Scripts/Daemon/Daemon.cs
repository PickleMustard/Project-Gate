using Godot;
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
    Level = GetTree().GetNodesInGroup("Level")[0];
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
      communicator.SpawnEnemy((Resource)EnemySpawnLocations[location], generatedEnemy);
      used_locations.Add(location);
    }
  }

  public void SpawnPlayerCharacter(Resource Tile, Character generatedCharacter)
  {
    PlayerTeamCharacter character = ResourceLoader.Load<PackedScene>("res://Assets/Units/playerteamcharacter.tscn").Instantiate() as PlayerTeamCharacter;
    GenericCharacterBanner characterBanner = (GenericCharacterBanner)ResourceLoader.Load<PackedScene>("res://User-Interface/generic_character_banner.tscn").Instantiate();
    Level.AddChild(character, true);

    string name = character.Name;
    character.ReplaceBy(generatedCharacter, true);
    character.QueueFree();
    generatedCharacter.Name = name;
    ((CommunicationBus)Engine.GetSingleton("CommunicationBus")).AddCharacter(character, characterBanner);

    //characterBanner.UpdateCharacterName(generatedCharacter.CharacterName);
    //characterBanner.UpdateIconBanner(generatedCharacter.GetCharacterIcon());
    //characterBanner.UpdateMovementRemaining(generatedCharacter.GetDistanceRemaining());
    //characterBanner.UpdateHeapPriority(generatedCharacter.HeapPriority);
    //characterBanner.SizeFlagsVertical = Control.SizeFlags.ShrinkCenter;
    generatedCharacter.Connect(generatedCharacter.GetSignalList()[1]["name"].ToString(), characterBanner.GetUpdateMovementCallable());
    generatedCharacter.Connect(generatedCharacter.GetSignalList()[2]["name"].ToString(), characterBanner.GetUpdateHeapPriorityCallable());
    generatedCharacter.AddToGroup("PlayerTeam");
    generatedCharacter.SetupCharacter();
    generatedCharacter.Call("SetPosition", Tile);
  }
  private void AddPlayerTeam()
  {
    List<int> used_location = new List<int>();
    Character generatedCharacter = generator.GenerateCharacter("GeneratedCharacter0");
    communicator.SpawnPlayerCharacter((Resource)PlayerTeamSpawnLocations[0], generatedCharacter);
    generatedCharacter = generator.GenerateCharacter("GeneratedCharacter0");
    communicator.SpawnPlayerCharacter((Resource)PlayerTeamSpawnLocations[1], generatedCharacter);
    generatedCharacter = generator.GenerateCharacter("GeneratedCharacter0");
    communicator.SpawnPlayerCharacter((Resource)PlayerTeamSpawnLocations[2], generatedCharacter);

    //((GodotObject)PlayerTeamSpawnLocations[0]).Call("SpawnCharacter");
    //((GodotObject)PlayerTeamSpawnLocations[1]).Call("SpawnCharacter");
    //((GodotObject)PlayerTeamSpawnLocations[2]).Call("SpawnCharacter");
  }
}
