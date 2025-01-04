using Godot;
using System;

public partial class EndTurnButton : Button
{
  public override void _Ready() {
    //GD.Print("End Turn Button: ", GetTreeStringPretty());
    CharacterTurnController TurnControl = (CharacterTurnController)(GetTree().GetNodesInGroup("TurnController")[0]);
    Connect(SignalName.ButtonUp, TurnControl.GetEndTurnCall());
  }

}
