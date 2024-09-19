/* Basic Interface to define the different behaviors a weapon can have when hitting an opponent
 *
 */
public interface IOnHitBehavior {
  void CalculateOnHit(int baseDamage, Character target); //Might need to use params type[] list if different implementations need different amounts of parameters
}
