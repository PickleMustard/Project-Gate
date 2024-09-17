using Godot;
using Godot.Collections;
using System;

public partial class Enemy : Character
{
  Node TileGrid;
  Node level;

  Func<Node> test;
  public override void _Ready()
  {
    SetupCharacter();
    Callable SetPositionCall = new Callable(this, "SetPosition");
    level = GetNode<Node>("/root/Top/Level");
    TileGrid = level.GetChildren()[0];
    if (TileGrid.HasMethod("AddEnemyCall"))
    {
      TileGrid.Call("AddEnemyCall", SetPositionCall);
    }
    AddToGroup("Enemies");
    //GD.Print("Health at beninging ", currentHealth);
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
