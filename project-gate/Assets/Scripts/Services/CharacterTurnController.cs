using Godot;
using Godot.Collections;
using System.Collections.Generic;
using System.Collections;
using System;

public partial class CharacterTurnController : Node
{
  public static CharacterTurnController Instance { get; private set; }

  [Signal]
  public delegate void EndTurnSignalEventHandler();

  //UI Object to display FinalizedMovementQueue and FlexibleMovementQueue
  private TurnOrderBanner OrderBanner;

  //Ordered List of what character is moving next in the turn (Can hold duplicates)
  Queue<Character> FinalizedMovementQueue;
  //Similar list to FinalizeMovementQueue, allows characters positions to shift before finalization
  Queue<Character> FlexibleMovementQueue;
  //Max Heap of Alive characters ordered by their move priority
  PriorityQueue<Character, float> PriorityUpdateHeap;
  //Collection of all Characters currently alive on the level
  List<Character> AliveCharacterList;
  //Callable to the movement calculation function under each alive character
  List<Callable> CharacterMovementUpdateFunctions;
  //Current Character in the turn order | Updates the movement controller
  [Export]
  Character CurrentCharacter;
  //Unit Controller Object to update which character to control
  UnitControl unitControl;
  Node TileGrid;
  Node level;
  Callable StartLevelTurnController;

  private Texture2D endturnicon;

  public override void _EnterTree()
  {
    AddToGroup("TurnController");
  }

  public override void _Ready()
  {
    endturnicon = ResourceLoader.Load("res://Assets/Icons/End-Turn-Icon.png") as Texture2D;
    StartLevelTurnController = new Callable(this, "StartLevel");
    CharacterMovementUpdateFunctions = new List<Callable>();
    FinalizedMovementQueue = new Queue<Character>();
    FlexibleMovementQueue = new Queue<Character>();
    AliveCharacterList = new List<Character>();
    PriorityUpdateHeap = new PriorityQueue<Character, float>();
    unitControl = GetTree().GetNodesInGroup("UnitControl")[0] as UnitControl;
    Node Daemon = (Node)Engine.GetSingleton("Daemon");
    Daemon.Connect("StartTurnController", StartLevelTurnController);
    Instance = this;
  }

  /* Upon start of the level, after the characters are generated, calls this function
   * Will generate the initial priority queue then create the turn list from it
   *
   */
  public void StartLevel()
  {
    OrderBanner = GetTree().GetNodesInGroup("TurnOrderBanner")[0] as TurnOrderBanner;
    GD.Print("Starting Turn Controller, ", AliveCharacterList.Count);
    CallDeferred("CalculateMovementQueue");
    CallDeferred("FinalizeMovementQueue");
    CallDeferred("NextCharacter");
    CallDeferred("GenerateOrderBanner");
  }

  public Character NextCharacter()
  {
    GD.Print("Next Character");
    GD.Print(FinalizedMovementQueue.Count);
    if (FinalizedMovementQueue.Count > 0)
    {
      Character next_character = FinalizedMovementQueue.Dequeue();
      next_character.ResetDistanceRemaining();
      next_character.MakeMainCharacter();
      CurrentCharacter = next_character;
      if (next_character.IsInGroup("Enemies"))
      {
        next_character.Call("RunAI");
      }
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
  public void EndCharacterTurn()
  {
    CommunicationBus s = (CommunicationBus)Engine.GetSingleton("CommunicationBus");
    s.EmitSignal(CommunicationBus.SignalName.IdentifyStrayNode);
    PrintOrphanNodes();

    //GD.Print("Ending ", CurrentCharacter.ToString(), "'s turn");
    CurrentCharacter = NextCharacter();
    if (CurrentCharacter != null)
    {
      GD.Print("Another Character Available");
      GD.Print(unitControl, "| ", CurrentCharacter);
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
      GenerateOrderBanner();
    }
    else
    {
      GD.Print("End of global turn");
      EndGlobalTurn();
      CurrentCharacter = NextCharacter();
      unitControl.UpdateCurrentCharacter(CurrentCharacter);
      GenerateOrderBanner();
    }
  }

  public Callable GetEndTurnCall()
  {
    return new Callable(this, "EndCharacterTurn");
  }

  public void AddCharacterToMovementQueue(Character SpawnedCharacter)
  {
    FlexibleMovementQueue.Enqueue(SpawnedCharacter);
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
    FinalizeMovementQueue();
  }

  private void FinalizeMovementQueue()
  {
    GD.Print("Finalizing Movement Queue");
    FinalizedMovementQueue = new Queue<Character>(FlexibleMovementQueue);
    GD.Print("FinalizeMovementQueue: ", FinalizedMovementQueue.Count, "| Flexible: ", FlexibleMovementQueue.Count);
    FlexibleMovementQueue.Clear();
    CalculateMovementQueue();
  }

  private void GenerateOrderBanner()
  {
    Texture2D[] IconBannerArray = new Texture2D[7];
    if(FinalizedMovementQueue.Count >= TurnOrderBanner.ICONS_IN_BANNER) {
      IconBannerArray[0] = CurrentCharacter.Icon;
      Character[] MovementArray = FinalizedMovementQueue.ToArray();
      for(int i = 0; i < 6; i++) {
        IconBannerArray[i+1] = MovementArray[i].Icon;
      }
      OrderBanner.UpdateTurnOrderBanner(IconBannerArray);
    } else {
      IconBannerArray[0] = CurrentCharacter.Icon;
      Character[] MovementArray = FinalizedMovementQueue.ToArray();
      for(int i = 0; i < MovementArray.Length; i++) {
        IconBannerArray[i+1] = MovementArray[i].Icon;
      }
      IconBannerArray[MovementArray.Length] = endturnicon;
      OrderBanner.UpdateTurnOrderBanner(IconBannerArray);
    }

  }

  private void CalculateMovementQueue()
  {
    GD.Print("Calculate Movement Queue");
    PriorityUpdateHeap.Clear();
    foreach (Character c in AliveCharacterList)
    {
      c.ResetPriority();
      PriorityUpdateHeap.Enqueue(c, c.GetCurrentHeapPriority());
    }

    while (PriorityUpdateHeap.Count > 0)
    {
      Character calculatingCharacter = PriorityUpdateHeap.Dequeue();
      (bool ShouldEnqueue, bool ShouldRequeue) = calculatingCharacter.UpdateMovementCalculation();
      if (ShouldEnqueue)
      {
        AddCharacterToMovementQueue(calculatingCharacter);
      }
      if (ShouldRequeue)
      {
        calculatingCharacter.RequeueingCharacter();
        PriorityUpdateHeap.Enqueue(calculatingCharacter, calculatingCharacter.GetCurrentHeapPriority());
      }
    }

  }

  public void AddUpdateCharacterMovementCallable(Callable updateFunction)
  {
    CharacterMovementUpdateFunctions.Add(updateFunction);
  }

  public void RemoveUpdateCharacterMovementCallable(Callable updateFunction)
  {
    CharacterMovementUpdateFunctions.Remove(updateFunction);
  }

  public void AddCharacterToTurnController(Character character)
  {
    GD.Print("Adding Character to movment queue: ", character);
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
