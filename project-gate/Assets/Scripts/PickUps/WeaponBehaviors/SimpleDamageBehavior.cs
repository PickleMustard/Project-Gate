using Godot;
using System;

using ProjGate.Character;

namespace ProjGate.Pickups
{
  [GlobalClass]
  public partial class SimpleDamageBehavior : Resource, IOnHitBehavior
  {
    void IOnHitBehavior.CalculateOnHit(BaseWeapon attackingWeapon, BaseCharacter Attacker, Resource targetedLocation, Node TileGrid)
    {
      BaseCharacter target = targetedLocation.Call("GetCharacterOnTile").AsGodotObject() as BaseCharacter;
      float proficiency = 1.0f / (int)Attacker.proficiencies[(int)attackingWeapon.weaponType];
      target.RecieveDamage((int)Math.Round(attackingWeapon.BaseDamage * proficiency));
    }
  }
}
