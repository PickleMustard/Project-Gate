/* Basic Interface to define the different behaviors a weapon can have when hitting an opponent
 *
 */
public interface IOnHitBehavior {
  public void CalculateOnHit(int baseDamage, float proficieny, Character target); //Might need to use params type[] list if different implementations need different amounts of parameters
}
