using Godot;
using System;

public partial class PlayerTeamCharacter : Character {

  public override void _Ready() {
    team = Character.CHARACTER_TEAM.player;
  }
}
