using Godot;

public partial class Character : Node3D
{
  [Signal]
  public delegate void UpdateMainCharacterEventHandler();

  [Export]
  public int TotalDistance = 8;

  private int distanceRemaining {get; set;}

  public bool isMoving {get; set;} = false;

  public override void _Ready()
  {
    distanceRemaining = TotalDistance;
    InputHandler i_handle = GetNode<Node>("/root/Top/input_handler") as InputHandler;
    i_handle.UpdateCharacter += MakeMainCharacter;
    Node3D UnitMovement = GetNodeOrNull<Node3D>("/root/Top/pivot/unit_movement");
    if (UnitMovement.HasMethod("GetUpdateCharacterSignal"))
    {
      Connect(SignalName.UpdateMainCharacter, (Callable)UnitMovement.Call("GetUpdateCharacterSignal"));
    }
    EmitSignal(SignalName.UpdateMainCharacter, this);
  }


  public void MakeMainCharacter()
  {
    GD.Print("Emitting Signal");
    EmitSignal(SignalName.UpdateMainCharacter, this);
  }

  public int GetDistanceRemaining() {
    return distanceRemaining;
  }

  public void DecrementDistanceRemaining(int decrementor) {
    distanceRemaining -= decrementor;
  }

  public void ResetDistanceRemaining() {
    distanceRemaining = TotalDistance;
  }

}
