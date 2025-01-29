namespace ProjGate.TargetableEntities
{
  public interface IAbility
  {
    public bool CanExecute(AbilityContext context);
    public void Execute(AbilityContext context);
  }
}
