using Godot;

public partial class SecondTestBehavior : Resource, IOnHitBehavior
{
  public void CalculateOnHit(Weapon attackingWeapon, Character Attacker, Resource TargetedLocation, Node TileGrid)
  {
    GD.Print("Second Test Behavior OnHit Method");

  }
}
