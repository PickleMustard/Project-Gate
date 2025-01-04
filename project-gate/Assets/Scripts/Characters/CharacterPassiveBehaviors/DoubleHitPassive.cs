using Godot;
public partial class DoubleHitPassive : Resource, IGenericPassiveBehavior
{
  void IGenericPassiveBehavior.ExecutePassiveBehavior(params Variant[] WorldState)
  {
    for (int i = 0; i < WorldState.Length; i++)
    {
      GD.Print(WorldState[i]);
    }

  }
}
