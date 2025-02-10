using Godot;

using ProjGate.Services;

/// <summary>
/// <class> EndTurnButton </class> defines functionality of UI button that ends a player character's turn
/// </summary>
public partial class EndTurnButton : Button
{
  [Signal]
  public delegate void EndTurnButtonPressedEventHandler();
  public override void _Ready()
  {
    Connect(SignalName.ButtonUp, new Callable(this, MethodName.ButtonPressedEvent));
    Connect(SignalName.EndTurnButtonPressed, CommunicationBus.Instance.EndTurnEvent);
  }
  private void ButtonPressedEvent()
  {
    EmitSignal(SignalName.EndTurnButtonPressed);
  }
}
