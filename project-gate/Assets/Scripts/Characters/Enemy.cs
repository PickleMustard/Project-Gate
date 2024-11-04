using Godot;
using Godot.Collections;
using System;

public partial class Enemy : Character
{
  [Signal]
  public delegate void EnemyKilledEventHandler();

  Node level;

  public override void _Ready()
  {
    Connect(SignalName.EnemyKilled, CommunicationBus.Instance.GetEnemyKilledEventCallable());
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

  protected override void KillCharacter()
  {
    CharacterTurnController.Instance.RemoveCharacterFromTurnController(this);
    CharacterTurnController.Instance.RemoveUpdateCharacterMovementCallable(updateMovementCalcs);
    AudioStreamPlayer3D player = (AudioStreamPlayer3D)FindChild("character_death");
    player.Play();
    Visible = false;
    EmitSignal(SignalName.EnemyKilled, this);
  }
}
