using Godot;
using System;

public partial class PlayerTeamCharacter : Character {

  public override void _Ready() {
    //testWeaponGenerationName = "testweapon";
    SetupCharacter();
    team = Character.CHARACTER_TEAM.player;
    AddToGroup("PlayerTeam");
  }
}
