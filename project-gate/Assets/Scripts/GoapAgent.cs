using Godot;
public partial class GoapAgent : Node {
  private FSM stateMachine;

  private FSM.FSMState idelState;

  private void createIdleState() {
    idelState = (fsm, gd, test) => {

    };
  }

}
