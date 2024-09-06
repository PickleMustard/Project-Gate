using Godot;
using System.Collections.Generic;
public partial class FSM : Node {
  private Stack<FSMState> stateStack = new Stack<FSMState> ();
  public delegate void FSMState (FSM fsm, GodotObject gd);
  public void Update (GodotObject gd) {
    if(stateStack.Peek() != null) {
      stateStack.Peek().Invoke(this, gd);
    }
  }

  public void pushState(FSMState state) {
    stateStack.Push (state);
  }

  public void popState() {
    stateStack.Pop();
  }
}

