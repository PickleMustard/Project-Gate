using Godot;
using System.Collections.Generic;
using System;

public partial class Daemon : Node {
  private Godot.Collections.Array EnemySpawnLocations;
  private Godot.Collections.Array PlayerTeamSpawnLocations;

  private Node Level; //Takes the level generated signal to start its background processes

  public static Daemon Instance;

  [Signal]
  public delegate void StartTurnControllerEventHandler();

  public override void _EnterTree() {
    Instance = this;
    Engine.RegisterSingleton("Daemon", this);
  }
  public override void _Ready() {
    EnemySpawnLocations = new Godot.Collections.Array();
    PlayerTeamSpawnLocations = new Godot.Collections.Array();
    Level = GetTree().GetNodesInGroup("Level")[0];
    Level.Connect("LevelGenerated", new Callable(this, "LevelStartProcesses"));
  }

  public void SetEnemySpawnLocations(Godot.Collections.Array EnemySpawnLocations) {
    this.EnemySpawnLocations = EnemySpawnLocations;
  }

  public void SetPlayerTeamSpawnLocations(Godot.Collections.Array PlayerTeamSpawnLocations) {
    this.PlayerTeamSpawnLocations = PlayerTeamSpawnLocations;
  }

  public void AddEnemySpawnLocation(Resource newLocation) {
    EnemySpawnLocations.Add(newLocation);
  }

  public void AddPlayerSpawnLocation(Resource newLocation) {
    PlayerTeamSpawnLocations.Add(newLocation);
  }

  private void LevelStartProcesses() {
    GD.Print("The Daemon awakens");
    CallDeferred("GenerateStartingEnemies");
    CallDeferred("AddPlayerTeam");
    CallDeferred("LevelStartFinalization");
  }

  private void LevelStartFinalization() {
    EmitSignal(SignalName.StartTurnController);
  }

  private void GenerateStartingEnemies() {
    List<int> used_locations = new List<int>();
    GodotObject rnd = Engine.GetSingleton("GlobalSeededRandom");
    for(int i = 0; i < 1; i++) {
      int location = (int)rnd.Call("GetInteger", 0, EnemySpawnLocations.Count);
      while(used_locations.Contains(location)) {
        location = (int)rnd.Call("GetInteger", 0, EnemySpawnLocations.Count);
      }
      GD.Print("Attemptin to spawn something at ", location);
      ((GodotObject)EnemySpawnLocations[location]).Call("SpawnCharacter");
      used_locations.Add(location);
    }
  }

  private void AddPlayerTeam() {
    List<int> used_location = new List<int>();
    ((GodotObject)PlayerTeamSpawnLocations[0]).Call("SpawnCharacter");
  }
}
