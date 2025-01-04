using Godot;
/* Basic Interface to define the different behaviors a weapon can have when hitting an opponent
 *
 */
public interface IOnHitBehavior {
  public void CalculateOnHit(Weapon attackingWeapon, Character Attacker, Resource TargetedLocation, Node TileGrid); //Might need to use params type[] list if different implementations need different amounts of parameters
}
