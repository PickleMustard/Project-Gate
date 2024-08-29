using Godot;
using System.Collections.Generic;

public partial class CharacterTurnController : Node
{
  public static CharacterTurnController Instance { get; private set; }

  [Signal]
  public delegate void EndTurnSignalEventHandler();

  List<Character> MovementQueue;
  List<Callable> UpdateMovementCalcs;
  Character CurrentCharacter;
  UnitControl unitControl;

  public override void _Ready()
  {
    UpdateMovementCalcs = new List<Callable>();
    MovementQueue = new List<Character>();
    unitControl = GetNodeOrNull<UnitControl>("/root/Top/pivot/UnitControl");

    Instance = this;
  }

  public Character NextCharacter()
  {
    if (MovementQueue.Count > 0)
    {
      Character next_character = MovementQueue[0];
      MovementQueue.RemoveAt(0);
      next_character.ResetDistanceRemaining();
      return next_character;
    }
    else
    {
      return null;
    }

  }

  public void EndTurn()
  {
    GD.Print("Ending ", CurrentCharacter.ToString(), "'s turn");
    UpdateCharacterMovement();
    CurrentCharacter = NextCharacter();
    if (CurrentCharacter != null)
    {
      CurrentCharacter.ResetDistanceRemaining();
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
    } else {
      while(CurrentCharacter == null) {
        UpdateCharacterMovement();
        CurrentCharacter = NextCharacter();
      }
      CurrentCharacter.ResetDistanceRemaining();
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
    }
  }

  public void AddCharacterToMovementQueue(Character SpawnedCharacter)
  {
    MovementQueue.Add(SpawnedCharacter);
  }

  public void RemoveCharacterFromMovementQueue(Character DeaddedCharacter)
  {
    if (MovementQueue.Contains(DeaddedCharacter))
    {
      MovementQueue.Remove(DeaddedCharacter);
    }
  }

  public void UpdateCharacterMovement()
  {
    for (int i = 0; i < UpdateMovementCalcs.Count; i++)
    {
      UpdateMovementCalcs[i].Call();
    }
  }

  public void AddUpdateCharacterMovementCallable(Callable updateFunction)
  {
    UpdateMovementCalcs.Add(updateFunction);
  }

  public void RemoveUpdateCharacterMovementCallable(Callable updateFunction)
  {
    UpdateMovementCalcs.Remove(updateFunction);
  }

}
