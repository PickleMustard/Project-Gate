using Godot;
using System.Collections.Generic;
using System.Collections;

public partial class CharacterTurnController : Node
{
  public static CharacterTurnController Instance { get; private set; }

  [Signal]
  public delegate void EndTurnSignalEventHandler();

  List<Character> MovementQueue;
  PriorityQueue<Character, float> PriorityUpdateHeap;
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

  public void StartLevel() {

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

  /* At the end of a characters turn, it returns control back to the Turn CharacterTurnController
   * The Turn Controller calls for the next character in the movement queue if it exists
   * If it doesn't exist, all characters for the Global turn have moved
   * Thus, all movement stats should be rerolled
   */
  public void EndTurn()
  {
    GD.Print("Ending ", CurrentCharacter.ToString(), "'s turn");
    CurrentCharacter = NextCharacter();
    if (CurrentCharacter != null)
    {
      CurrentCharacter.ResetDistanceRemaining();
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
    } else {
      EndGlobalTurn();
      CurrentCharacter = NextCharacter();
      CurrentCharacter.ResetDistanceRemaining();
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
    }
  }

  public void AddCharacterToMovementQueue(Character SpawnedCharacter)
  {
    MovementQueue.Add(SpawnedCharacter);
  }

  public void AddCharacterToMovementHeap(Character character) {
    PriorityUpdateHeap.Enqueue(character, character.HeapPriority);
  }

  public void RemoveCharacterFromMovementQueue(Character DeaddedCharacter)
  {
    if (MovementQueue.Contains(DeaddedCharacter))
    {
      MovementQueue.Remove(DeaddedCharacter);
    }
  }

  public void EndGlobalTurn()
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
