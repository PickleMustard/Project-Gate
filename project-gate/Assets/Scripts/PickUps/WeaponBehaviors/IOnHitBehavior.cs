using Godot;

using ProjGate.TargetableEntities;
/* Basic Interface to define the different behaviors a weapon can have when hitting an opponent
 *
 */
namespace ProjGate.Pickups
{
  public interface IOnHitBehavior
  {
    public void CalculateOnHit(BaseWeapon attackingWeapon, BaseCharacter Attacker, Resource TargetedLocation, Node TileGrid); //Might need to use params type[] list if different implementations need different amounts of parameters
  }
}
