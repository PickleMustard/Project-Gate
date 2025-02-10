using Godot;

using ProjGate.Services;

public partial class  GenericCharacterInteractionStateButton: Button
{
  [Signal]
  public delegate void ChangeStateButtonPressedEventHandler();
  public override void _Ready()
  {
    Connect(SignalName.ButtonUp, new Callable(this, MethodName.ButtonPressedEvent));
    Connect(SignalName.ChangeStateButtonPressed, CommunicationBus.Instance.UpdateCharacterInteractionStateEvent);
  }
  /// <summary>
  /// <func> ButtonPressedEvent </func> emits a ChangeState event signal when the UI button is pressed
  /// </summary>
  private void ButtonPressedEvent()
  {
    EmitSignal(SignalName.ChangeStateButtonPressed, this.Name);
  }


}

