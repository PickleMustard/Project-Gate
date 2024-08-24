using Godot;
using System.Collections.Generic;
using System.Collections;

public partial class CharacterTurnController : Node3D
{

  Queue<Character> MovementQueue;
  List<Callable> UpdateMovementCalcs;

  public override void _Ready()
  {
    UpdateMovementCalcs = new List<Callable>();
    MovementQueue = new Queue<Character>();
  }

  public Character NextCharacter() {
    Character next_character = MovementQueue.Dequeue();
    next_character.ResetDistanceRemaining();
    return next_character;
  }

  public void EndTurn(){

  }

  public void UpdateCharacterMovement() {
    for(int i = 0; i < UpdateMovementCalcs.Count; i++) {
      UpdateMovementCalcs[i].Call();
    }
  }

}
