using Godot;
using System;

using ProjGate.TargetableEntities;

namespace ProjGate.Pickups
{
  public partial class IncreaseDamageFromDistanceRemaining : Resource, IOnHitBehavior
  {
    void IOnHitBehavior.CalculateOnHit(BaseWeapon attackingWeapon, BaseCharacter Attacker, Resource TargetedLocation, Node TileGrid)
    {

      BaseCharacter target = TargetedLocation.Call("GetCharacterOnTile").AsGodotObject() as BaseCharacter;
      float proficiency = 1.0f / (int)Attacker.proficiencies[(int)attackingWeapon.weaponType];
      float distanceRemaining = (float)Attacker.distanceRemaining / (float)Attacker.TotalDistance;
      GD.Print("Distance Remaining Coefficient: ", distanceRemaining, "| ", (float)Attacker.distanceRemaining, ", ", (float)Attacker.TotalDistance);
      target.RecieveDamage((int)Math.Round(attackingWeapon.BaseDamage * proficiency));
    }
  }
}
