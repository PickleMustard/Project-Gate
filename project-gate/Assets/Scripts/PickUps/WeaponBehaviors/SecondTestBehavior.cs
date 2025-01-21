using Godot;

namespace ProjGate.Pickups
{
  public partial class SecondTestBehavior : Resource, IOnHitBehavior
  {
    public void CalculateOnHit(BaseWeapon attackingWeapon, Character Attacker, Resource TargetedLocation, Node TileGrid)
    {
      GD.Print("Second Test Behavior OnHit Method");

    }
  }
}
