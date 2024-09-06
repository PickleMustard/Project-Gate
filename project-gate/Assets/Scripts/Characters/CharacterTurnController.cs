using Godot;
using System.Collections.Generic;
using System.Collections;

public partial class CharacterTurnController : Node
{
  public static CharacterTurnController Instance { get; private set; }

  [Signal]
  public delegate void EndTurnSignalEventHandler();

  //Ordered List of what character is moving next in the turn (Can hold duplicates)
  Queue<Character> MovementQueue;
  //Max Heap of Alive characters ordered by their move priority
  PriorityQueue<Character, float> PriorityUpdateHeap;
  //Characters current alive on the level
  List<Character> AliveCharacterList;
  //Callable to the movement calculation function under each alive character
  List<Callable> UpdateMovementCalcs;
  //Current Character in the turn order | Updates the movement controller
  [Export]
  Character CurrentCharacter;
  //Unit Controller Object to update which character to control
  UnitControl unitControl;
  Node TileGrid;
  Node level;
  Callable StartLevelTurnController;

  public override void _Ready()
  {
    StartLevelTurnController = new Callable(this, "StartLevel");
    UpdateMovementCalcs = new List<Callable>();
    MovementQueue = new Queue<Character>();
    AliveCharacterList = new List<Character>();
    PriorityUpdateHeap = new PriorityQueue<Character, float>();
    unitControl = GetNodeOrNull<UnitControl>("/root/Top/pivot/UnitControl");
    level = GetNode<Node>("/root/Top/Level");
    level.Connect(level.GetSignalList()[1]["name"].ToString(), StartLevelTurnController);

    Instance = this;
  }

  /* Upon start of the level, after the characters are generated, calls this function
   * Will generate the initial priority queue then create the turn list from it
   *
   */
  public void StartLevel()
  {
    GD.Print("Starting Turn Controller");
    CallDeferred("EndGlobalTurn");
  }

  public Character NextCharacter()
  {
    if (MovementQueue.Count > 0)
    {
      Character next_character = MovementQueue.Dequeue();
      next_character.ResetDistanceRemaining();
      next_character.MakeMainCharacter();
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
      GD.Print("Another Character Available");
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
    }
    else
    {
      GD.Print("End of global turn");
      EndGlobalTurn();
      CurrentCharacter = NextCharacter();
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
    }
  }

  public void AddCharacterToMovementQueue(Character SpawnedCharacter)
  {
    MovementQueue.Enqueue(SpawnedCharacter);
  }

  public void AddCharacterToMovementHeap(Character character)
  {
    PriorityUpdateHeap.Enqueue(character, character.HeapPriority);
  }

  /* On the end of a global turn (i.e. every character has used all their turns) create a new ordered heap based on new priorities
   * Generate new movement queue
   *
   */
  public void EndGlobalTurn()
  {
    GD.Print("Character List Size: ", AliveCharacterList.Count);
    int caution = 0;
    foreach(Character c in AliveCharacterList) {
      c.ResetPriority();
      GD.Print("CurrentHeapPriority b4 enqueue: ", c.CurrentHeapPriority);
      PriorityUpdateHeap.Enqueue(c, c.CurrentHeapPriority);
    }
    GD.Print("heap: ", PriorityUpdateHeap.Count);
    while(PriorityUpdateHeap.Count > 0 && caution < 1000) {
      GD.Print("Going through heap");
      Character calclatee = PriorityUpdateHeap.Dequeue();
      bool ShouldRequeue = calclatee.UpdateMovementCalcs();
      GD.Print(ShouldRequeue);
      if(ShouldRequeue) {
        calclatee.RequeueingCharacter();
        GD.Print("Requeueing with Priority: ", calclatee.CurrentHeapPriority);
        PriorityUpdateHeap.Enqueue(calclatee, calclatee.CurrentHeapPriority);
      }
      caution++;
    }
    GD.Print("MovementQueue: ", MovementQueue.Count);
    GD.Print(MovementQueue.ToString());
    CurrentCharacter = NextCharacter();

  }

  public void AddUpdateCharacterMovementCallable(Callable updateFunction)
  {
    UpdateMovementCalcs.Add(updateFunction);
  }

  public void RemoveUpdateCharacterMovementCallable(Callable updateFunction)
  {
    UpdateMovementCalcs.Remove(updateFunction);
  }

  public void AddCharacterToTurnController(Character character)
  {
    AliveCharacterList.Add(character);
  }

  public void RemoveCharacterFromTurnController(Character character)
  {
    if (AliveCharacterList.Contains(character))
    {
      AliveCharacterList.Remove(character);
    }
  }

}
