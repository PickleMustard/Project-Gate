namespace ProjGate.Character {
  public static class AbilityExtenisions {
    public static bool TryGetBehavior<T>(this BaseAbility ability, out T result) where T: class, IAbility {
      result = ability as T;
      return result != null;
    }
  }
}
