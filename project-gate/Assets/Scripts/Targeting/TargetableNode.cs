using Godot;

namespace ProjGate.Targeting {
  [GlobalClass]
  public partial class TargetableNode : Node {
    Vector3 GetPosition() {
      return new Vector3(0,0,0);
    }

    public bool IsValidFor(ProjGate.Character.BaseAbility.ABILITY_TYPE types) {
      return true;
    }
  }
}
