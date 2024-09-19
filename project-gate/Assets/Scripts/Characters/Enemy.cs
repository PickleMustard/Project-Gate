using Godot;
using Godot.Collections;
using System;

public partial class Enemy : Character
{
  Node TileGrid;
  Node level;

  public override void _Ready()
  {
    SetupCharacter();
    team = Character.CHARACTER_TEAM.enemy;
    Callable SetPositionCall = new Callable(this, "SetPosition");
    AddToGroup("Enemies");
    GD.Print("Testing ClassDB assignment during runtime: ", ClassDB.ClassExists("Character"));
    var test = ResourceLoader.Load("res://Assets/Scripts/User-Interface/GenericCharacterBanner.cs") as CSharpScript;
    var second_test = (GodotObject)test.New();
    GD.Print(second_test);
    GD.Print(second_test.HasMethod("UpdateHeapPriority"));
  }


  public void RunAI()
  {
    Array<Node> children = GetChildren();
    GD.Print("Enemy children: ", children);
    if (children[2].HasMethod("RunAI"))
    {
      children[2].Call("RunAI");
    }
  }
}
