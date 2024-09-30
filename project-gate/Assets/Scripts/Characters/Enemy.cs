using Godot;
using Godot.Collections;
using System;

public partial class Enemy : Character
{
  Node TileGrid;
  Node level;

  public override void _Ready()
  {
    team = Character.CHARACTER_TEAM.enemy;
    Callable SetPositionCall = new Callable(this, "SetPosition");
    var test = ResourceLoader.Load("res://Assets/Scripts/User-Interface/GenericCharacterBanner.cs") as CSharpScript;
  }

  //Needs to set the behaviors of the Goap agent underneath the node
  public void SetAIBehaviors(Godot.Collections.Array behaviors) {

  }

  public void RunAI()
  {
    Node aiAgent = this.FindChild("agent", true, false);
    if (aiAgent.HasMethod("RunAI"))
    {
      aiAgent.Call("RunAI");
    }
  }
}
