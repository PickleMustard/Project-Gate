using Godot;
namespace ProjGate.TargetableEntities
{
  public interface IGenericPassiveBehavior
  {
    void ExecutePassiveBehavior(params Variant[] WorldState);
  }
}
